## Audit system 

## Requirde versions of soft:
g++ version > 5.00

## Description
Audit.cpp - main file  
constant.h - dict with process system names  

## How to start

```bash
g++ Audit.cpp -o audit  

```
by Pid  

```bash
./audit PID
```
by name  
```bash
./audit $(pidof -s <name>)
```
## How to find in logs:
by Pid  
```bash 
sudo cat ./logs.log | grep <pid>
```
by name 
```bash 
sudo cat ./logger.log | grep $(pidof -s <name>)
```
