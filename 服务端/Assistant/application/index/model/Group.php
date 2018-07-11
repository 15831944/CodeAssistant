<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2018/6/8 0008
 * Time: 下午 3:36
 */

namespace app\index\model;

use think\Model;

class Group extends Model
{
    // 定义一个私有的数据表变量
    protected $table = "group";

    //设置数据表主键
    protected $pk    = 'GroupId';
}