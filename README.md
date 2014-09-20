查找泄露的gmail
===============

网上查找泄露的gmail太慢，用c写了个查找

===============
1. 用libevent做socket server
2. 用uthash 存储数据

===============
使用make &&
./find test.txt
写了个php界面查询
===============
valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./find test.txt 
===============
记录点gdb调试技巧

#加上参数
set args test.txt
#断点
break find.c:200
#调试子进程
set follow-fork-mode child
#运行
run
#修改变量值
set email = "lijianwei_123@126.com"
#跳转到多少行运行 打乱原先执行顺序
jump find.c:200


==============
测试程序
./find_test 10 100


