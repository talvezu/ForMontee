WIP
```
enable camera from WSL is not trivial, following this procedure enables
listing camera via lsusb
but /dev/Video is not available, resulting with
>v4l2-ctl -l
Cannot open device /dev/video0, exiting.

 sudo service udev status
 * systemd-udevd is not running

 this issue is not trivial. leaviing it as is for now.
```
[enable usb camera on wsl - youtube](https://www.youtube.com/watch?v=I2jOuLU4o8E&t=74s)

[enable usb camera on wsl - documentation](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)

``` terminal output
> usbipd wsl attach --busid 8-2
usbipd: info: Using default WSL distribution 'Ubuntu-22.04'; specify the '--distribution' option to select a different one.
> usbipd wsl list
BUSID  VID:PID    DEVICE                                                        STATE
8-2    1bcf:28c4  Depstech webcam, Depstech webcam MIC                          Attached - Ubuntu-22.04
```

``` terminal output detach
> usbipd wsl detach --busid 8-2
> usbipd wsl list
BUSID  VID:PID    DEVICE                                                        STATE
8-2    1bcf:28c4  Depstech webcam, Depstech webcam MIC                          Not attached
```
