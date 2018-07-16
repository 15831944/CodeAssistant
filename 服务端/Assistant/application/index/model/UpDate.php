<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2018/7/13 0013
 * Time: 10:25
 */

namespace app\index\model;

use think\Model;

class UpDate extends Model
{
    // 定义一个私有的数据表变量
    protected $table = "update";

    //设置数据表主键
    protected $pk    = 'UpDateId';
}