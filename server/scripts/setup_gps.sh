#!/bin/bash

# Verifica se o script está sendo executado como root
if [ "$(id -u)" != "0" ]; then
    echo "Este script deve ser executado como superusuário (root)!" >&2
    echo "Tentando reiniciar com sudo..." >&2
    # Tenta reiniciar o script com sudo
    sudo "$0" "$@"
    exit $?
fi

# Define as variáveis para ID do Vendedor e do Produto
VENDOR_ID="1546"
PRODUCT_ID="01a7"
SYMLINK_NAME="ublox_gps"

# Cria o arquivo de regras udev no diretório correto
echo "Criando a regra udev para o dispositivo GPS U-Blox..."

# Verifica se o arquivo já existe e cria ou atualiza conforme necessário
UDEV_RULE_FILE="/etc/udev/rules.d/99-ublox-gps.rules"
if [ -f "$UDEV_RULE_FILE" ]; then
    echo "O arquivo de regras já existe. Adicionando a regra ao arquivo existente."
else
    echo "O arquivo de regras não existe. Criando um novo arquivo de regras."
    touch "$UDEV_RULE_FILE"
fi

# Adiciona a regra ao arquivo de regras udev
echo "SUBSYSTEM==\"tty\", ATTRS{idVendor}==\"$VENDOR_ID\", ATTRS{idProduct}==\"$PRODUCT_ID\", SYMLINK+=\"$SYMLINK_NAME\"" | tee -a $UDEV_RULE_FILE

# Recarrega as regras udev e aciona as mudanças
echo "Recarregando as regras udev e acionando as mudanças..."
udevadm control --reload-rules
udevadm trigger

echo "Configuração completa. Um symlink '$SYMLINK_NAME' foi criado para o dispositivo GPS U-Blox."
