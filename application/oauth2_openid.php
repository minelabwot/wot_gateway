<?php
/*
本文件位置
$redirect_url= "http://israel.duapp.com/weixin/oauth2_openid.php";

URL
https://open.weixin.qq.com/connect/oauth2/authorize?appid=wx6292681b13329528&redirect_uri=http://israel.duapp.com/weixin/oauth2_openid.php&response_type=code&scope=snsapi_base&state=1#wechat_redirect
*/
$code = $_GET["code"];


//$userinfo = getUserInfo($code);

$openid = getUserId($code);
$deviceid=getDeviceId($openid);
$deviceList=getDeviceList($deviceid);

function getUserId($code)
{
  $appid = "wx250d00817156085c";
  $appsecret = "50c6b69c831f811d4d723990abec6173";

    //oauth2的方式获得openid
  $access_token_url = "https://api.weixin.qq.com/sns/oauth2/access_token?appid=$appid&secret=$appsecret&code=$code&grant_type=authorization_code";
  $access_token_json = https_request($access_token_url);
  $access_token_array = json_decode($access_token_json, true);
  $openid = $access_token_array['openid'];

    return $openid;
}

function getDeviceId($openid)
{
    $appid = "wx250d00817156085c";
  $appsecret = "50c6b69c831f811d4d723990abec6173";

    //非oauth2的方式获得全局access token
    $new_access_token_url = "https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=$appid&secret=$appsecret";
    $new_access_token_json = https_request($new_access_token_url);
    $new_access_token_array = json_decode($new_access_token_json, true);
    $new_access_token = $new_access_token_array['access_token'];

    $deviceIdUrl="https://api.weixin.qq.com/device/get_bind_device?access_token=$new_access_token&openid=$openid";
    $deviceIdUrl_json=https_request($deviceIdUrl);
    $deviceId_array = json_decode($deviceIdUrl_json, true);
    $deviceId = $deviceId_array['device_list'][0]['device_id'];
    return $deviceId;
}

function getDeviceList($MWID)
{
    $deviceListUrl="http://www.cloudsensing.cn:9071/WIFPa/MWAttribute/"."$MWID".".xml";
    $deviceList=https_request($deviceListUrl);
    return $deviceList;
}

function https_request($url)  //method=get
{
  $curl = curl_init();
  curl_setopt($curl, CURLOPT_URL, $url);
  curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, FALSE);
  curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, FALSE);
  curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
  $data = curl_exec($curl);
  if (curl_errno($curl)) {return 'ERROR '.curl_error($curl);}
  curl_close($curl);
  return $data;
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<HEAD><TITLE>OAuth2.0认证</TITLE>
    <META charset=utf-8>
    <META name=viewport content="width=device-width, user-scalable=no, initial-scale=1">
    <script type="text/javascript">
function msg()
{
sessionStorage.openid="<?php echo $openid;?>";
sessionStorage.deviceid="<?php echo $deviceid;?>";
var open=sessionStorage.openid;
    var device=sessionStorage.deviceid;
document.getElementById("formid1").action="http://lixiaopengtest.sinaapp.com/index.php/?command=001&openid="+open+"&deviceid="+device;
document.getElementById("formid1").method="post";
document.getElementById("formid1").submit();

document.getElementById("formid2").action="http://lixiaopengtest.sinaapp.com/index.php/?command=002&openid="+open+"&deviceid="+device;
document.getElementById("formid2").method="post";
document.getElementById("formid2").submit

document.getElementById("formid3").action="http://lixiaopengtest.sinaapp.com/index.php/?command=003&openid="+open+"&deviceid="+device;
document.getElementById("formid3").method="post";
document.getElementById("formid3").submit();

document.getElementById("formid4").action="http://lixiaopengtest.sinaapp.com/index.php/?command=004&openid="+open+"&deviceid="+device;
document.getElementById("formid4").method="post";
document.getElementById("formid4").submit();

document.getElementById("formid5").action="http://lixiaopengtest.sinaapp.com/index.php/?command=005&openid="+open+"&deviceid="+device;
document.getElementById("formid5").method="post";
document.getElementById("formid5").submit();
}
</script>
<style  type="text/css">
#big {
            margin: 0 auto;
      width:350px;
        }
form{margin-bottom: 0;}
</style>
</HEAD>
<body>
<iframe id="id_iframe" name="id_iframe" style="display:none;"></iframe>
<form id="formid1"  >
<input type="image" src="stop.jpg" alt="Submit" onclick="stop()"/>
</form>

<div id="big">

<div style="text-align:center">
<form id="formid2" target="id_iframe" >
<input type="image" name="top" src="top.jpg" alt="Submit" onclick="msg()"/>
</form>
</div>

<div style="width:350px;">
<form id="formid3" target="id_iframe" style="float:left" >
<input type="image" name="left" src="left.jpg" alt="Submit" onclick="msg()"/>
</form>
<form id="formid4" target="id_iframe" style="float:right;">
<input type="image" name="right" src="right.jpg" alt="Submit" onclick="msg()"/>
</form>
</div>

<div style="text-align:center;clear:both;">
<form id="formid5" target="id_iframe">
<input type="image" name="bottom" src="bottom.jpg" alt="Submit" onclick="msg()"/>
</form>
</div>

</div>

</body>
</html>
