<?php
/**
 * Created by PhpStorm.
 * User: Cnlyz
 * Date: 2018/6/9
 * Time: 11:19
 */

namespace app\index\model;


use think\Model;

class Version extends Model
{
    // 定义一个私有的数据表变量
    protected $table = "version";

    //设置数据表主键
    protected $pk    = 'VersionId';
}