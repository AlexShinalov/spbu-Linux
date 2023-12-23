# Backup daemon

## How to build
```bash
mkdir build
cd ./build
cmake ..
make
cd ..
```

## How to start daemon
```bash
cp ./backup-daemon.service /etc/systemd/system/
cp ./backup.ini /etc/
systemctl daemon-reload
systemctl start backup-daemon
systemctl enable backup-daemon
```

## How to pause / continue daemon
```bash
systemctl kill -s SIGTSTP backup-daemon
systemctl kill -s SIGCONT backup-daemon
```

## How to terminate daemon
```bash
systemctl stop backup-daemon
```

## How to see logs

```bash
journalctl -u backup-deamon
```
