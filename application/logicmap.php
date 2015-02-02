<?php
  include "get_all.php";

  $temperature = "当前温度：".$tem_ret;
  $humidity = "当前湿度".$wet_ret;

    function getmapping($message){

        global $temperature,$humidity;

        $mapArray = array("温度" => "$temperature", "湿度" => "$humidity","wendu" => "$temperature","拍照" => "ok");
        foreach($mapArray as $key => $value){
            if(stripos($message,$key)===false){
                $returnValue = "目前只能查询温度和湿度哦";
            }
            else{
                parse_str("returnValue=".$value);
                break;
          }
        }
        return $returnValue;

    }
?>
