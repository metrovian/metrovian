#!/bin/bash
SHARE_DIR="/var/lib/metrovian/mids"
GROUP="metrovian"
USER_NAME="${SUDO_USER:-$(logname)}"

sudo systemctl stop smbd
if ! sudo pdbedit -L | grep -q "^$USER_NAME:"; then
  sudo smbpasswd -a $USER_NAME
fi

sudo groupadd -f $GROUP
sudo usermod -aG $GROUP $USER_NAME
sudo mkdir -p $SHARE_DIR
sudo chown root:$GROUP $SHARE_DIR
sudo chmod 2775 $SHARE_DIR

if ! grep -q "^\[$GROUP\]" /etc/samba/smb.conf; then
  sudo tee -a /etc/samba/smb.conf > /dev/null <<EOF

[$GROUP]
path = $SHARE_DIR
browseable = yes
read only = no
valid users = @$GROUP
force group = $GROUP
create mask = 0664
directory mask = 2775
EOF
fi

sudo systemctl start smbd
sudo systemctl status smbd --no-pager
sudo systemctl enable smbd
