
# Container / Image Cleanup

sudo docker stop server_container
sudo docker stop client_container
sudo docker stop dvc_container

sudo docker rm server_container
sudo docker rm client_container
sudo docker rm dvc_container

sudo docker rmi client_image
sudo docker rmi server_image
sudo docker rmi dvc_image


# Take User Inputs

sResponse=
echo -n "Enter name of server file (For Default:string.txt, then just press Enter): "
read sResponse
if [ -n "$sResponse" ]; then
	serverFile=$sResponse
else
	serverFile="string.txt"
fi


cResponse=
echo -n "Enter name of client file (For Default:string.txt, then just press Enter): "
read cResponse
if [ -n "$cResponse" ]; then
	clientFile=$cResponse
else
	clientFile="string.txt"
fi


pResponse=
echo -n "Enter Port Number (For Default:8087, then just press Enter): "
read pResponse
if [ -n "$pResponse" ]; then
	portNo=$pResponse
else
	portNo="8087"
fi

# Take Backup and Modify files based on user inputs

cp Dockerfile_data_volume Dockerfile_data_volume_backup
cp compilerscript_client.sh compilerscript_client_backup.sh
cp compilerscript_server.sh compilerscript_server_backup.sh


#Add both the files to the Data Volume
sed -i 's/Userfile/'$serverFile'/g' Dockerfile_data_volume_backup
if [ $serverFile != $clientFile ]; then
sed -i '/ADD/ a\ADD '$clientFile' /data/'$clientFile'' Dockerfile_data_volume_backup
fi

#Add the Client File to Client Compiler Script & Server File to Server Compiler Script
sed -i 's/string.txt/'$clientFile'/g' compilerscript_client_backup.sh
sed -i 's/string.txt/'$serverFile'/g' compilerscript_server_backup.sh

#Change Port Number in both Compiler Client and Compiler Server
sed -i 's/port_no/'$portNo'/g' compilerscript_server_backup.sh
sed -i 's/port_no/'$portNo'/g' compilerscript_client_backup.sh


# Image Builds

sudo docker build -t server_image --file=Dockerfile_server .
sudo docker build -t client_image --file=Dockerfile_client .
sudo docker build -t dvc_image --file=Dockerfile_data_volume_backup .

# Execute image on containers
sudo docker create -v /data --name dvc_container dvc_image
sudo docker run -itd --name=server_container --hostname=serverHost --volumes-from=dvc_container -P server_image
ip_add=$(sudo docker inspect --format '{{ .NetworkSettings.IPAddress }}' $(sudo docker ps -q))
sed -i s/serverHost/$ip_add/ compilerscript_client_backup.sh
sudo docker run -itd --name=client_container --volumes-from=dvc_container --link server_container -P client_image

# Testing
sudo docker ps -a
sleep 30
sudo docker ps -a
sudo docker logs server_container
sudo docker logs client_container

okay_cases=$(sudo docker logs client_container | grep 'OK' | wc -l )
echo $okay_cases
if [ $okay_cases -ge 10 ]
then
	echo "Passed"
else
	echo "Failed"
fi

#Remove the backups


rm Dockerfile_data_volume_backup
rm compilerscript_client_backup.sh
rm compilerscript_server_backup.sh   


# Container / Image Cleanup

sudo docker stop server_container
sudo docker stop client_container
sudo docker stop dvc_container

sudo docker rm server_container
sudo docker rm client_container
sudo docker rm dvc_container

sudo docker rmi client_image
sudo docker rmi server_image
sudo docker rmi dvc_image

 
