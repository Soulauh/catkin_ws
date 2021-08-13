#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
from std_msgs.msg import String
import os
import sys
import json
import threading
import socket
import time
import array
import logging

HOST = '0.0.0.0'
PORT = 12000

ROS_NODE_NAME = 'SocketNode'
rospy.init_node(ROS_NODE_NAME, anonymous=True, log_level=rospy.INFO)
rospy.sleep(2.0)  # The necessary delay for init

lock = threading.Lock()
run_lock = threading.Lock()
exec_lock = threading.Lock()

current_sock = None
EXE_RUN_TIME_OUT = 20


def _list_to_array(act_list):
    return array.array("d", act_list)


def _dict_to_bytes(dict_msg):
    return bytes(json.dumps(dict_msg))


def async_do_job(func, args=None):
    threading.Thread(target=func, args=args).start()


joinData = ""
def handle_sck_data(_datas, conn):
    global joinData
    if (_datas.find("cmd", 0, 10) != -1):
        joinData  = "" 
    joinData += _datas
    try:       
        data = json.loads(joinData.decode("utf-8"))  
        rospy.loginfo(data) 
        cmd = data['cmd']
        if cmd == 'set_all_servo':
            async_do_job(set_all_servo, args=(data['id'], data['angle'], data['time'], conn, data['sendback']))
        elif cmd == 'set_head_servo':
            async_do_job(set_head_servo, args=(data['angle'], data['time'], conn,))
        else:
            print("[WARN] WIP...")

    except Exception as err:
        logging.error(err)

def thread_handle_connect(conn):
    global current_sock
    global con_is_reset
    while not rospy.is_shutdown():
        try:  
              
            data = conn.recv(8192*3, 0x40)        
            rospy.loginfo("recviving.....")      
            if not data:
                print('[WARN] lost socket connection ...')
                conn.close()
                current_sock = None
                NODE.walkflag = False
                con_is_reset = True
                break   
            handle_sck_data(data, conn)       
        except Exception as e:
            err = e.args[0]
            if err == 'timed out':
                continue
            else:
                conn.close()
                current_sock = None
                con_is_reset = True
                NODE.walkflag = False
                break

def get_socket():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
    server.setsockopt(socket.SOL_TCP, socket.TCP_KEEPIDLE, 5)
    server.setsockopt(socket.SOL_TCP, socket.TCP_KEEPINTVL, 3)
    server.setsockopt(socket.SOL_TCP, socket.TCP_KEEPCNT, 3)
    server.bind((HOST, PORT))
    server.listen(1)
    server.settimeout(None)
    return server

             
def main():
    server = None
    global current_sock
    while not rospy.is_shutdown():
        if current_sock:
            time.sleep(1)
            continue
        else:
            print('listening...')
            server =  get_socket()
            rospy.set_param("socket_is_connected", False)
            conn, addr = server.accept()
            rospy.set_param("socket_is_connected", True)
            server.close()
        current_sock = conn
        print('handling connection from %s' % (addr,))
        threading.Thread(target=thread_handle_connect, args=(conn,)).start()


if __name__ == '__main__':
    main()


