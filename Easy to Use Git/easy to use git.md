# Easy to use git to push your codes and projects

```bash
#create a new repository on the command line
cd D:\FudanCoding\repo\FudanCoding
git init

#add
git add -A #add all your project
git add file.cpp #only add a file

#commit
git commit -m "first_version" #record

#first time to connect your github repository
git remote add origin git@github.com:Kingsblessing/FudanCoding.git #SSH
https://github.com/Kingsblessing/FudanArch26-CPU.git #HTTPS

#if error:src refspec does not match any branch
git branch -M main

#push
git push -u origin main



#push an existing repository from the command line
git remote add origin git@github.com:Kingsblessing/Fudan-CS10005-ICS-Lab.git
git branch -M main
git push -u origin main



#if error:SSL certificate OpenSSL verify result: unable to get local issuer certificate (20)
git config --local http.sslVerify false # temporarily disable SSL for the current project
git push -u origin main
git config --local http.sslVerify true # recover SSL certificate
```
