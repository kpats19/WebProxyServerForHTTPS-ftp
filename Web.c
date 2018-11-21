#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/types.h> 
#include<string.h> 
#include<stdlib.h>
#include<netdb.h> 
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<signal.h>
int welcomeSocket,proxySocket,newSocket,dataSocket,clientLen, nBytes,counter=1,con=0,sdata=0,cdata=0;
long int portNum;
char requestPath[1024],request[1024],Host_Name[100]="",ip[100],response2[512],response3[10240],path[1024],aPath[1024],Frequest[1024];
long int remotePort=80;
struct sockaddr_in serverAddr;
struct sockaddr_storage serverStorage;
socklen_t addr_size;
 char myHint[5]="http";
 

int strln(char * str){
	int lengh=0;
	for (lengh=0;*str!='\0';*str++,lengh++);
	return lengh;
}
void strcp(char * des,char * src){
	memset (des, 0, strln(des));
  int lengh;
	for (lengh=0;lengh<strln(src);lengh++)
	des[lengh]=src[lengh];
}
void strca(char * des,char * src){
  int dlengh,lengh;
	for (dlengh=strln(des),lengh=0;lengh<strln(src);lengh++,dlengh++)
	des[dlengh]=src[lengh];
}
void init(){
  welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);	 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portNum);
  serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
  int B=bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
}

void init_remote_server(){
	int okay;
	if(!sdata) {proxySocket = socket(AF_INET, SOCK_STREAM, 0);	sdata=1;} 
  	else {dataSocket = socket(AF_INET, SOCK_STREAM, 0);sdata=0;}
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(remotePort);
  serverAddr.sin_addr.s_addr = inet_addr(ip);
  addr_size = sizeof serverAddr;
  if(!cdata) {okay=connect(proxySocket, (struct sockaddr *) &serverAddr, addr_size); cdata=1;}
  else{ okay=connect(dataSocket, (struct sockaddr *) &serverAddr, addr_size);cdata=0;}
  printf("\nCONNECT: %d %d %d \t",okay,sdata,cdata);
  }

void dnslookup(){
	
	int sockfd,rv,sfd;  
    struct addrinfo hints, *servinfo, *rp;
    struct sockaddr_in *h;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
 
    if ( (rv = getaddrinfo( Host_Name , myHint , &hints , &servinfo)) != 0) 
    {
       strcp(Host_Name,"Host Name or Service is not Know !!");
       strcp(ip,"(0.0.0.0)");
       return;
    }
    for(rp = servinfo; rp != NULL; rp = rp->ai_next) 
    {
    	h = (struct sockaddr_in *) rp->ai_addr;
        sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
        if (sfd == -1)continue;
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
        {
        	strcp(ip ,inet_ntoa( h->sin_addr ) );
            break; 
        }             
		close(sfd);      
    }
    freeaddrinfo(servinfo);
}

void parse(){
	int count=0,host=0,offSet=0,copy=0;
	if(request[count]== 67)	{con=1;}		
	else{
			if(request[4]==102){strcp(myHint,"ftp"); remotePort=21;}	
			con=0;
			for (count=0;count <= strln(request);count++)
			{	
				if(request[count]== 47)
				offSet++;
				if(offSet==2)
				{	
					Host_Name[host]=request[count+1];
					host++;
					if(request[count+2]== 47||request[count+2]== 58)break;
				}
			}offSet=0;
			int check=1;
			for (count=0,host=0;count <= strln(request);count++)
			{	
				if(request[count]== 47)
				offSet++;
				if(offSet>2)
				{	
					path[host]=request[count];
					host++;
					if(request[count+1]==32)break;
				}
			}offSet=0;
			for (count=0,host=0;count <= strln(request);count++)
			{	
				if(request[count]== 32)
				offSet++;
				if(offSet>0)
				{	
					aPath[host]=request[count];
					host++;
					if(request[count+1]==32)break;
				}
			}
			
			printf("\nPATH:%s",path);
			
			for (count=0,host=0;count <= strln(request);count++,host++)
			{					
					if(request[count]== 10)break;
					requestPath[host]=request[count];
			}
			dnslookup();
		}
}

void filter_response(){
	int count=0;
	for (count=0;count <= strln(response2);count++)
	{
		if(response2[count]== 60)
		{	printf("\n<<");
			if(response2[count+1]==33&&(response2[count+2]==100||response2[count+2]==68))
			{ printf("!!");
			int temp=0;
				for(temp=0;temp<=strln(response2)-count;temp++)
						response3[temp]=response2[count+temp];
			}
		}	
	}
}
void make_request(){
		memset(Frequest, 0, 1024);
		strca(Frequest,requestPath);
		strca(Frequest,"\n");		
		strca(Frequest,"Host:");
		strca(Frequest,Host_Name);
		strca(Frequest,"\r\nConnection: close\r\n\r\n");
		}
void request_remote_server(){
		memset (response2, 0, 512);
		init_remote_server();
		make_request();		
		write(proxySocket,Frequest,strln(Frequest)+1);
		int readByte=0;
		while(readByte=read(proxySocket,response2,512)){
		printf("%s",response2);
		if(write(newSocket,response2,readByte)<0){close(newSocket);break;}
		memset (response2, 0, 512);
		}
		memset (Frequest, 0, 1024);
		memset (requestPath, 0, 1024);
		}
		
void request_ftp_remote_server(){

		char response[1048576],htmlOpen[700]="<!DOCTYPE html><html><head><meta http-equiv='Content-Type' content='text/html; charset=UTF-8'><style>table { font-family: arial, sans-serif; border-collapse: collapse; width: 100%;""}td, th { border: 1px solid #dddddd; text-align: left; padding: 8px;}tr:nth-child(even) { background-color: #dddddd;}</style></head><body><h1><u><b>Index of</b></u></h1><table><tr>",htmlClose[40]="</body></html>",trOpen[5]="<tr>",trClose[6]="</tr>",thOpen[5]="<th>",thClose[6]="</th>",request[1024],buffer[1024],ftpUser[50]="USER anonymous\r\n",ftpPass[50]="PASS chrome@example.com\r\n",ftpSyst[10]="SYST\r\n",ftpPwd[10]="PWD\r\n",sizechar[1024]="SIZE ",ftpCwd[1024]="CWD ",ftpRetr[1024]="RETR ",filename[256],ftpList[12]="LIST -l\r\n";
		int nBytes,i,kin,done,checkSize=0,checkCwd=0,offSet=0,take=0,fileHandle;
		int intPortF,intPortS,Ftake=0,Stake=0,once=1,onceS=1;
		char portF[5],portS[5],size[20]; 
		long int ldSize=0;
		init_remote_server();
		make_request();
		printf("%s",Frequest);
		write(proxySocket,Frequest,strln(Frequest));
		read(proxySocket, buffer, 1024);
		read(proxySocket, buffer, 1024);
		memset (buffer, 0, 1024);
		
		write(proxySocket,ftpUser,strln(ftpUser));
  		read(proxySocket, buffer, 1024);
  		printf("\nUSER:%s",buffer);
  		memset (buffer, 0, 1024);
		
		write(proxySocket,ftpPass,strln(ftpPass));
		while(done=read(proxySocket,buffer, 1024))
		{
			for(i=0;i<=strln(buffer);i++){
				if(buffer[i]==50&&buffer[i+1]==51&&buffer[i+2]==48&&buffer[i+3]==32) {done=0;break;} 
			}
			if (done==0)break;
		}
		printf("\nPASS:%s",buffer);
  		
		memset (buffer, 0, 1024);
		
		write(proxySocket,ftpSyst,strln(ftpSyst));
		read(proxySocket, buffer, 1024);
		printf("\nSYST:%s",buffer);
  		
		memset (buffer, 0, 1024);
  
  		write(proxySocket,ftpPwd,strln(ftpPwd));
  		read(proxySocket, buffer, 1024);
  		printf("\nPWD:%s",buffer);
  		memset (buffer, 0, 1024);
		printf("@@PATH:%s\tA",path);
		strca(sizechar,path);
		strca(sizechar,"\r\n");		
  		write(proxySocket,sizechar,strln(sizechar));
  		read(proxySocket, buffer, 1024);
		printf("\nSIZE:%s",buffer);
		offSet=0;
		Ftake=0;
 		if(buffer[0]==50){
 			checkSize=1;
			for (i=0;i<strln(buffer);i++)
			{	
				if(buffer[i]==13)break;
				if(buffer[i]==32)offSet++;
				if(offSet==1){
					if(once){i=i+1;once=0;}
					size[Ftake]=buffer[i];
					Ftake++;
				}
			}
		}
		sscanf(size,"%ld",&ldSize);
   		memset (buffer, 0, 1024);
		
		strca(ftpCwd,path);
		strca(ftpCwd,"\r\n");	
		write(proxySocket,ftpCwd,strln(ftpCwd));
  		read(proxySocket, buffer, 1024);
  		printf("\nCWD:%s",buffer);
  		if(buffer[0]==50) checkCwd=1;
  		memset (buffer, 0, 1024);
  	
  		write(proxySocket,"PASV\r\n",6);
  		read(proxySocket, buffer, 1024);
  		printf("\nPASV:%s",buffer);
  		Ftake=0; Stake=0;
  		offSet=0;once=1;
  		for(i=0;i<strln(buffer);i++)
  		{	printf("IN");
		 	if(buffer[i]==44)offSet++;
		 	if(buffer[i]==41)break;
		 	if (offSet==4)	
		 	{
		 		if(once){i=i+1;once=0;}
		 		portF[Ftake]=buffer[i];
		 		Ftake++;
		 	}
			if(offSet==5)
			{ 				
			if(onceS){i=i+1;onceS=0;}
			portS[Stake]=buffer[i];
			Stake++;
			}
		}
		printf("portF:%s",portF);
		printf("portS:%s",portS);
		sscanf(portF,"%d",& intPortF);	
		sscanf(portS,"%d",& intPortS);	
		printf("intportF:%d\tportS:%d",intPortF,intPortS);
		remotePort=(intPortF*256)+intPortS;
		printf("REMOTEPORT:%ld",remotePort);
		init_remote_server();
		int readByte,wrt;
  		char *file;
  	
		if(checkSize==0&&checkCwd==1){
				file = malloc(1024);
  		memset (file, 0, 1024);
  		
  		 memset (buffer, 0, 1024);
         write(proxySocket,ftpList,strln(ftpList));
         read(proxySocket, buffer, 1024);
			printf("\nLIST:\n%s",buffer);
			strcp(response,htmlOpen);
			char temp[1024];
			while(readByte=read(dataSocket,file,1024)){
				
			printf("\nreadByte:%d\n%s",readByte,file);
			for(i=0;i<=readByte;i++){
			       if(file[i]==13&&file[i+1]==10){
			  	   strca(response,thOpen);
			  	   strca(response,"<a href=""");
			  	   strca(response,aPath);
			  	  	int slash=strln(aPath);
			  	  	if(aPath[slash-1]!=47)
			  	   	strca(response,"/");
			   	  
			   	   char spTemp[1024];
			   	   int j;
			   	   for(j=0;j<=strln(temp);j++)
				   spTemp[j]=temp[j+1];			   		
			   	   strca(response,spTemp);
			   	   strca(response,""">");
			   	   strca(response,temp);
			   	   strca(response,"</a>");
			   	   strca(response,thClose);
			  	   strca(response,trClose);
			  	   strca(response,trOpen);
				   memset(temp,0,1024);
				   take=0;
				}
			   if(file[i]==32&&file[i+1]!=32){
			   		strca(response,thOpen);
			   		strca(response,temp);
			   		strca(response,thClose);
			   		memset(temp,0,1024);	
			   		take=0;
			   }
			   temp[take]=file[i];
			   take++;
   			   
   			}
   			memset (file, 0, 1024);
   		}
		   strca(response,htmlClose);
		   write(newSocket,response,strln(response));
		   printf("\nResponse:\n%s",response);
		   memset (response, 0, 1048576);
		   memset (path, 0, 1024);
			}
		
		if(checkSize==1&&checkCwd==0){
		printf("\nIn retr\nPath:%s",path);
		strca(ftpRetr,path);
		strca(ftpRetr,"\r\n");	
  		write(proxySocket,ftpRetr,strln(ftpRetr));
		read(proxySocket, buffer, 1024);
		printf("RETR:%s",buffer);
		memset (buffer, 0, 1024);
		take=0;
		
		file = malloc(1024);
		  
		  int countR=0,countW=0;
		  int readByte=0,wrt=0;
		  while(1){

		  if((ldSize-countR)<1024){countR=countR+readByte;readByte=read(dataSocket,file,1024);
		   wrt=write(newSocket,file,readByte);break;}

		   readByte=read(dataSocket,file,1024);
		  if(wrt=write(newSocket,file,readByte)<0) {close(newSocket);break;}
		  	printf("WRT:%d",wrt);
		   countR=countR+readByte;
		  }
	}
	memset (portF, 0, 5);
	memset (portS, 0, 5);
	memset (path, 0, 1024);
	close(proxySocket);
	close(dataSocket);
	close(fileHandle);	
	cdata=0;
	sdata=0;
	strcp(myHint,"http"); 
	remotePort=80;
}
void go()
{	
	while(1)
	{		char finalResponse[2000];
			int try=1,count,take;
			int L=listen(welcomeSocket,50);
			FILE *fd;
			if(L==0)printf("Listening\n");
			else printf("Error\n");
			while (try)
			{	
			addr_size = sizeof serverStorage;
			newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
			read(newSocket,request,1024);
			printf("\nRequest:%s",request);
			parse();
			if((!con)&&(myHint[0]==104))request_remote_server();
			if((!con)&&(myHint[0]==102))request_ftp_remote_server();
			
			memset (requestPath, 0, 1024);
			memset (request, 0, 1024);
			memset (Frequest, 0, 1024);
			memset (Host_Name, 0, 100);
    		memset (ip, 0, 100);
			memset (response2, 0, 512);	
			memset (path, 0, 1024);
			close(newSocket);
			close(proxySocket);
			try=0;
			}
	}
}     

int main()
{		
	printf("PLEASE ENTER A PORT:");
	scanf("%ld",&portNum);
	signal(SIGPIPE, SIG_IGN);
	init();
	go();		
}
