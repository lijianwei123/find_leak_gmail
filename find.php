<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<style type="text/css">
input{
    padding:0;
    margin:0;
}
#page {
    background: #fff;
    width:100%;
    height:800px;
    margin-top:100px;
}

#login-wrap
{
    width:500px;
    margin:auto auto;
    text-align:center;
}


#login-form #email, #login-form #login-btn{
    height:30px;
    line-height: 30px;
    padding-left:5px;
}

#login-form #email{
    width:300px;
}
</style>



<script src="http://f3.v.veimg.cn/f2e/gallery/jquery/1.9.1/jquery.min.js"></script>
<title>你春光外泄了吗？</title>


</head>

<body>
<?php
error_reporting(0);
$tips = "";

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    
    $email = trim(@$_POST['email']);
  
    if (!empty($email) && strpos($email, "@") !== false) {
         try {
            //连接超时
            $sock = stream_socket_client("tcp://168.192.122.31:9999", $errno, $errstr, 10);
            if(!$sock) {
                $tips =  "无法连接服务器!";
            } else {
                $str = $email. "\n";
        
                fwrite($sock, $str);
        
                $flag = fread($sock, 4096);

                //file_put_contents(__DIR__.'/email.txt', $email. '  '. $flag. PHP_EOL, FILE_APPEND);
        
                fclose($sock);
                
                $flag == 1 ? $tips = "你中奖了，赶紧穿上你的小裤衩吧！" : $tips = "你太丑了，懒得理你！";
        
            }
        } catch(Exception $e) {
            $tips =  "出现异常：". $e->getMessage();
        }
    } else {
        $tips = "小伙子,你想干嘛？";
    }
}
if ($tips) {
    $tips = "<span style='color:red;padding-left:10px;'>$tips</span>";
}
?>
    

<div id="page">
    <div id="login-wrap">
        <h2>查找<?=$tips?></h2>
        <form action="" id="login-form" name="login-form" method="post">
            <input type="text" value="johnny.r.stivers@gmail.com" name="email" id="email"> <input type="submit" value="GOOOOOO" id="login-btn"/>
        </form>
    </div>

</div>

</body>
</html>
