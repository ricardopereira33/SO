#!/bin/bash

rm -rf /home/$USER/.Backup
rm -f $HOME/bin/sobucli
rm -f $HOME/bin/sobusrv

clear
echo ""
echo "  _____________________________________________________"
echo " |                                                     |"
echo " |       O programa foi desinstalado com sucesso!      |"
echo " |-----------------------------------------------------|"
echo " |  AVISO: Por favor elimine a linha                   |"
echo " |         export PATH=\$PATH:\$HOME/bin no ficheiro     |"
echo " |         \$HOME/.bashrc. Esta foi criada aquando a    |"
echo " |         instalação do programa.                     |"
echo " |_____________________________________________________|"
echo ""