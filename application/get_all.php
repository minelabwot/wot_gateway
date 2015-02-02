<?php
    function get_temp(){
            $ch = curl_init();
            $tem_url='http://121.42.31.195:9071/WIFPa/ResourceData.xml/0001000054f08cfa?ResourceID=3';
            curl_setopt($ch, CURLOPT_URL, $tem_url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_HEADER, 0);
            $tem_data = curl_exec($ch);
            curl_close($ch);

            $tem_reg = '/<resvalue>(.*?)<\/resvalue>/';
            $tem_ret = is_valid($tem_data, $tem_reg);

          $tag_start_len = strlen('<resvalue>');
          $tag_end_len = strlen('<\/resvalue>');
          $gross_len = strlen($tem_ret);
          $tem_len = $gross_len - $tag_start_len - $tag_end_len;
          $tem_ret = substr($tem_ret, $tag_start_len, $tem_len+1);
            return $tem_ret;
    }
    // for test
    $tem_ret = get_temp();
//   echo $tem_ret;
    function get_wet(){
            $wet_url = 'http://121.42.31.195:9071/WIFPa/ResourceData.xml/0001000054f08cfa?ResourceID=4';
            $ch = curl_init();
            curl_setopt($ch, CURLOPT_URL, $wet_url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_HEADER, 0);
            $wet_data = curl_exec($ch);
            curl_close($ch);

            $wet_reg = '/<resvalue>(.*?)<\/resvalue>/';
            $wet_ret = is_valid($wet_data, $wet_reg);

            $tag_start_len = strlen('<resvalue>');
          $tag_end_len = strlen('<\/resvalue>');
          $gross_len = strlen($wet_ret);
          $wet_len = $gross_len - $tag_start_len - $tag_end_len;
          $wet_ret = substr($wet_ret, $tag_start_len, $wet_len+1);
            return $wet_ret;
    }
    // for test
    $wet_ret = get_wet();
//  echo $wet_ret;
    function is_valid($data, $reg)
    {
        preg_match($reg, $data, $ret);
        if (count($ret) != 0){
            return $ret[0];
        }
        else{
            return "null";
        }
    }

?>
