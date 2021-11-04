#!/bin/bash
./compile.sh
./SNTRA ../28Si/Neutron/ txt
./SNTRA ../30Si/Neutron/ txt
./SNTRA ../32S/Neutron/ txt
./SNTRA ../34S/Neutron/ txt
./SNTRA ../28Si/Proton/ txt
./SNTRA ../30Si/Proton/ txt
./SNTRA ../32S/Proton/ txt
./SNTRA ../34S/Proton/ txt
