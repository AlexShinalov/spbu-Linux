## Audit system 

## Requride versions of soft:
g++ version > 5.00

## How to start
by Pid
```bash
g++ Audit.cpp -o audit  
sudo ./audit 
```
by name  
```bash
./audit &(pidof -s <name>)
```
## How to find in logs:
by Pid  
```bash 
sudo cat ./logger.log | grep <pid>
```
by name 
```bash 
cat ./logger.log | grep $(pidof -s <name>)
```
