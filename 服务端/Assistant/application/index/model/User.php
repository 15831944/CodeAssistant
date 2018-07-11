<?php
/**
 * Created by PhpStorm.
 * User: Shadow
 * Date: 2018/3/9
 * Time: 16:36
 */

namespace app\index\model;

// 引入模型
use think\Model;

class User extends Model
{
    // 定义一个私有的数据表变量
    protected $table = "user";

    //设置数据表主键
    protected $pk    = 'UserId';
}