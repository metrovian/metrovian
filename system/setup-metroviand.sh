#!/bin/bash
sudo cp -arfv metroviand/metroviand.service /etc/systemd/system/metroviand.service
sudo systemctl daemon-reload
sudo systemctl start metroviand
sudo systemctl status metroviand --no-pager
sudo systemctl enable metroviand