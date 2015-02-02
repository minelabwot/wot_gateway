<?php
/*
    方倍工作室 http://www.cnblogs.com/txw1958/
    CopyRight 2013 www.doucube.com  All Rights Reserved
*/

include "logicmap.php";
include "post.php";

define("TOKEN", "weixin");
$wechatObj = new wechatCallbackapiTest();
if (isset($_GET['command'])) {
    $command=$_GET['command'];
    $userid=$_GET['openid'];
    $deviceid=$_GET['deviceid'];
    //echo $userid;
    post();
    //echo "user $userid send command $command to device $deviceid";
    //$deviceStatus=$wechatObj->deviceStatus($keyword);

    //echo $deviceStatus;

    //if($deviceStatus) $wechatObj->deviceStatusReset();
    //if($command) $wechatObj->deviceStatusReset();

}else{
    $wechatObj->responseMsg();
}

class wechatCallbackapiTest
{
    public function valid()
    {
        $echoStr = $_GET["echostr"];
        if($this->checkSignature()){
            echo $echoStr;
            exit;
        }
    }

    private function checkSignature()
    {
        $signature = $_GET["signature"];
        $timestamp = $_GET["timestamp"];
        $nonce = $_GET["nonce"];

        $token = TOKEN;
        $tmpArr = array($token, $timestamp, $nonce);
        sort($tmpArr);
        $tmpStr = implode( $tmpArr );
        $tmpStr = sha1( $tmpStr );

        if( $tmpStr == $signature ){
            return true;
        }else{
            return false;
        }
    }

    public function responseMsg()
    {
        $postStr = $GLOBALS["HTTP_RAW_POST_DATA"];

        if (!empty($postStr)){
            $postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
            $fromUsername = $postObj->FromUserName;
            $toUsername = $postObj->ToUserName;
            //$keyword = trim($postObj->Content);
            $keyword = trim($postObj->EventKey);
            $time = time();
            $textTpl = "<xml>
                        <ToUserName><![CDATA[%s]]></ToUserName>
                        <FromUserName><![CDATA[%s]]></FromUserName>
                        <CreateTime>%s</CreateTime>
                        <MsgType><![CDATA[%s]]></MsgType>
                        <Content><![CDATA[%s]]></Content>
                        <FuncFlag>0</FuncFlag>
                        </xml>";


            $contentStr = getmapping($keyword);

            if($contentStr==="ok") post();
            //$url="https://open.weixin.qq.com/connect/oauth2/authorize?appid=wx250d00817156085c&redirect_uri=http://lixiaopengtest.sinaapp.com/oauth2_openid.php&response_type=code&scope=snsapi_base&state=1#wechat_redirect";
            //$contentStr ="<a href=".$url.">test</a>";
            //$this->deviceStatusSet();
            $msgType = "text";
            //$contentStr = date("Y-m-d H:i:s",time());
            $resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $msgType, $contentStr);
            echo $resultStr;

        }else{
            echo "wrong";
            exit;
        }
    }

    function deviceStatus($keyword)
   {
     $mysql_host = SAE_MYSQL_HOST_M;
     $mysql_host_s = SAE_MYSQL_HOST_S;
     $mysql_port = SAE_MYSQL_PORT;
     $mysql_user = SAE_MYSQL_USER;
     $mysql_password = SAE_MYSQL_PASS;
     $mysql_database = SAE_MYSQL_DB;
     $keyword="dev0";

  $mysql_table = "deviceStatus";
    $mysql_state = "SELECT * FROM `".$mysql_table."` WHERE `device` LIKE  '%".$keyword."%' LIMIT 0 , 1";

  $con = @mysql_connect($mysql_host.':'.$mysql_port, $mysql_user, $mysql_password, true);
  if (!$con){
    die('Could not connect: ' . mysql_error());
  }
  mysql_query("SET NAMES 'UTF8'");
  mysql_select_db($mysql_database, $con);
  $result = mysql_query($mysql_state);
    $deviceStatus = "";
    while($row = mysql_fetch_array($result))
    {
        $deviceStatus = $row['status'];
        break;
    }
  mysql_close($con);
  return $deviceStatus;
    }

    function deviceStatusSet()            //参数为设备编号,标志位 置1
    {
     $mysql_host = SAE_MYSQL_HOST_M;
     $mysql_host_s = SAE_MYSQL_HOST_S;
     $mysql_port = SAE_MYSQL_PORT;
     $mysql_user = SAE_MYSQL_USER;
     $mysql_password = SAE_MYSQL_PASS;
     $mysql_database = SAE_MYSQL_DB;
     $keyword='dev0';

  $mysql_table = "deviceStatus";
    $mysql_state="UPDATE deviceStatus SET status = '1' WHERE device ='"."$keyword'";
  $con = @mysql_connect($mysql_host.':'.$mysql_port, $mysql_user, $mysql_password, true);
  if (!$con){
    die('Could not connect: ' . mysql_error());
  }
  mysql_query("SET NAMES 'UTF8'");
  mysql_select_db($mysql_database, $con);
    mysql_query($mysql_state);
    mysql_close($con);
    }

    function deviceStatusReset()            //参数为设备编号，标志位 置0
    {
     $mysql_host = SAE_MYSQL_HOST_M;
     $mysql_host_s = SAE_MYSQL_HOST_S;
     $mysql_port = SAE_MYSQL_PORT;
     $mysql_user = SAE_MYSQL_USER;
     $mysql_password = SAE_MYSQL_PASS;
     $mysql_database = SAE_MYSQL_DB;
     $keyword='dev0';

  $mysql_table = "deviceStatus";
    $mysql_state="UPDATE deviceStatus SET status = '0' WHERE device ='"."$keyword'";
  $con = @mysql_connect($mysql_host.':'.$mysql_port, $mysql_user, $mysql_password, true);
  if (!$con){
    die('Could not connect: ' . mysql_error());
  }
  mysql_query("SET NAMES 'UTF8'");
  mysql_select_db($mysql_database, $con);
    mysql_query($mysql_state);
    mysql_close($con);
    }
}


?>
