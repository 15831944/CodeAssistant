<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2018/6/8 0008
 * Time: 上午 10:27
 */

namespace app\index\model;


use think\Model;

class Project extends Model
{
    // 定义一个私有的数据表变量
    protected $table = "project";

    //设置数据表主键
    protected $pk    = 'ProjectId';
}