#!/bin/bash

active_running=$(systemctl status docker 2>&1 | grep -Po "active \(running\)")
if [[ ${active_running} = "active (running)" ]]; then
    printf "docker server is already ${active_running}\n"
    exit
fi
printf "${active_running}"
sudo apt update
sudo apt install docker.io -y
systemctl start docker
systemctl enable docker
active_running=$(systemctl status docker 2>&1 | grep -Po "active \(running\)")
if [[ ${active_running} = "active (running)" ]]; then
    printf "docker server is ${active_running}\n"
else
    printf "error occured during docker setup please check."    
fi

