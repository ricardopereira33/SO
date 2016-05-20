#!/bin/bash

echo "export PATH='$PATH:$HOME/bin'" >> $HOME/.bashrc
export PATH="$PATH:$HOME/bin"
mkdir -p $HOME/bin
cp sobucli $HOME/bin/sobucli
cp sobusrv $HOME/bin/sobusrv
clear

if [ -d "/home/$USER/.Backup" ]
then
	rm -r /home/$USER/.Backup
	mkdir /home/$USER/.Backup
	mkdir /home/$USER/.Backup/metadata
	mkdir /home/$USER/.Backup/data
else 
	mkdir /home/$USER/.Backup
	mkdir /home/$USER/.Backup/metadata
	mkdir /home/$USER/.Backup/data
fi

echo ""
echo "  _____________________________________________________"
echo " |                                                     |"
echo " |       A instalação foi concluída com sucesso!       |"
echo " |-----------------------------------------------------|"
echo " |                 Lista de comandos:                  |"
echo " |  |--------------------------------------------|     |"
echo " |  | . sobusrv ou make run                      |     |"
echo " |  | iniciar o servidor                         |     |"
echo " |  |--------------------------------------------|     |"
echo " |  | . sobucli backup <fich_1> <fich_2> <...>   |     |"
echo " |  | realizar o backup de um ou mais ficheiros  |     |"
echo " |  |--------------------------------------------|     |"
echo " |  | . sobucli restore <fich_1> <fich_2> <...>  |     |"
echo " |  | restaurar ficheiro(s) em backup            |     |"
echo " |  |--------------------------------------------|     |"
echo " |  | . sobucli delete <fich_1> <fich_2> <...>   |     |"
echo " |  | eliminar ficheiro(s) em backup             |     |"
echo " |  |--------------------------------------------|     |"
echo " |  | . sobucli gc                               |     |"
echo " |  | limpar o backup                            |     |"
echo " |  |--------------------------------------------|     |"
echo " |_____________________________________________________|"
echo ""