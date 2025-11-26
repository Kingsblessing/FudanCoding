# Easy to use git to push your codes and projects

```bash
#create a new repository on the command line
cd C:\Users\23954\Desktop\Fudan_EDU\FudanCoding
git init
#add
git add -A #add your project
git add file.cpp #only add a file
#commit
git commit -m "first_version" #record
#first time to connect your github repository
git remote add origin git@github.com:Kingsblessing/FudanCoding.git
#if error:src refspec does not match any branch
git branch -M main
#push
git push -u origin main
Fudan-Garbage-classify-2025.git

#push an existing repository from the command line
git remote add origin git@github.com:Kingsblessing/Fudan-CS10005-ICS-Lab.git
git branch -M main
git push -u origin main
```
