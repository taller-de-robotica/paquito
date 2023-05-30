# Problemas comunes

## wpa_supplicant

Nos ayudan las indicaciones en [Wifi Debian](https://linuxhint.com/3-ways-to-connect-to-wifi-from-the-command-line-on-debian/).

El archivo donde se pueden actualizar los datos de la red es ```/etc/wpa_supplicant.conf```

```
sudo wpa_passphrase <red> <contraseña> | sudo tee /etc/wpa_supplicant.conf
```

Agrega automaticamente la línea para la red.

## NetworkManager
```
Error: NetworkManager no se está ejecutando
```

Para reparar:

```
sudo apt install network-manager network-manager-gnome
sudo systemctl start NetworkManager     # Iniciar servicio
sudo systemctl enable NetworkManager    # Autoinicio durante boot
```

Así es posible conectarse con:

```
nmcli device wifi list
nmcli d wifi connect <red> password <contraseña>
```
