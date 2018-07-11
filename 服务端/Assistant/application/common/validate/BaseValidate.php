<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2017/11/26
 * Time: 11:47
 */

namespace app\common\validate;
/**
 * ============================================================================
 * HSH.Sass平台
 * 版权所有
 * 官网地址:
 * 交流社区:
 * 作者: Wolk He
 * 联系QQ:316625556
 * ============================================================================
 * 通用验证器
 */
use think\Validate;
use think\Exception;

class BaseValidate extends Validate
{
    protected $rule = [];
    protected $data = [];
    protected $flag = true;

    //预定义所有字段的错误提示
    public static $msg = [
        'require' => [
            'username'   => '请输入登录帐号',
            'email'      => '请输入邮箱地址',
            'phone'      => '请输入手机号码',
            'password'   => '请输入登录密码',
            'RePassword' => '请输入确认密码',
            'code'       => '请输入验证码',

            'verify'     => '请输入验证码',
            'confirm'    => '请输入确认密码',
            //......
        ],
        'regex' => [
            'email'    => '邮箱格式错误',
            'phone'    => '手机号码格式错误',
            'tel'      => '电话号码格式错误',
            'username' => '帐号格式错误',
            'IdCard'   => '邮箱格式错误',
            //......
        ],
    ];

    //预定义所有指定字段的正则表达式
    protected $preg = [
        'email'    => '/^([\w\.\_]+)@(\w{1,})(\.[a-z]{2,4})+$/',
        'phone'    => '/^1[34578]{1}[0-9]{9}$/',
        'tel'      => '',
        'username' => '/^[\w][\w\@\_\.]{5,31}$/',
        'IdCard'   => '/^\d{6}((19[2-9]\d[0-1]\d[0-3]\d(\d{4}|(\d{3}(\d|X|x))))|(\d{2}[0-1]\d[0-3]\d{4}))$/',
        //......
    ];

    /**
     **获取错误提示
     * @param  [str]            $data       验证类型
     * @param  [str]            $fields     指定字段
     * @return [type]                       [description]
     */
    private function getErrorMsg($ruleName, $field)
    {
        if(isset(self::$msg[$ruleName][$field]) && !empty(self::$msg[$ruleName][$field])){
            return self::$msg[$ruleName][$field];
        }else{
            throw new Exception($field.'错误提示尚未定义');
        }
    }

    /**
     **重置data属性值
     * @param  [arr]            $data       需要验证的数组
     * @param  [arr||str]       $fields     字段过虑
     * @param  boolean          $bool       true验证fields的字段，false验证除fields外的字段
     * @return [type]                       [description]
     */
    private function setData($data, $fields, $bool = true)
    {
        if(!empty($this->data) && !empty($data)){
            if(!array_intersect($this->data, $data)){
                if(!empty($fields)){
                    if(is_bool($fields)){
                        $bool = $fields;
                        $fields = $data;
                        $data = $this->data;
                    }else{
                        throw new Exception('Parameter error');
                    }
                }else{
                    $fields = $data;
                    $data = $this->data;
                }
            }
        }else if(empty($data)){
            $data = $this->data;
        }else{
            $this->data = $data;
        }
        if(empty($data)){
            throw new Exception('Missing parameter');
        }
        $rs[0] = $data;
        $rs[1] = $fields;
        $rs[2] = $bool;
        return $rs;
    }

    //添加错误提示
    public function setErrorMsg(array $data)
    {

    }

    //初始化
    public function data(array $data)
    {
        $this->data = $data;
        return $this;
    }

    /**
     **非空验证
     * @param  [arr]              $data       需要验证的数组
     * @param  [arr||str]          $fields     字段过虑
     * @param  boolean             $bool       true验证fields的字段，false验证除fields外的字段
     * @return [type]                          [description]
     */
    public function checkRequire(array $data=[], $fields=[], $bool = true)
    {
        if($this->flag){
            list($data, $fields, $bool) = $this->setData($data, $fields, $bool);
            $this->rule = [];
            $rule = [];
            if(!empty($fields)){
                if(is_string($fields)){
                    $fields = explode(',', $fields);
                }
                foreach($fields as $k=>$v){
                    if(array_key_exists($v, $data)){
                        if($bool){
                            $rule[] = [strtolower($v), 'require', $this->getErrorMsg('require', $v)];
                        }else{
                            unset($data[$v]);
                        }
                    }else{
                        if($bool){
                            unset($data[$v]);
                        }else{
                            $rule[] = [strtolower($v), 'require', $this->getErrorMsg('require', $v)];
                        }
                    }
                }
            }else{
                foreach($data as $k=>$v){
                    $rule[] = [strtolower($k), 'require', $this->getErrorMsg('require', $k)];
                }
            }
            $this->rule = $rule;
            if(false == $this->check($data)){
                $this->flag = false;
            }
        }
        return $this;
    }

    /**
     **正则验证
     * @param  [arr]            $data       需要验证的数组
     * @param  [arr||str]       $fields     字段过虑
     * @param  boolean          $bool       true验证fields的字段，false验证除fields外的字段
     * @return [type]                       [description]
     */
    public function checkRegex(array $data=[], $fields=[], $bool = true)
    {
        if($this->flag){
            list($data, $fields, $bool) = $this->setData($data, $fields, $bool);
            $this->rule = [];
            $rule = [];
            if(!empty($fields)){
                // if(is_string($fields)){
                //     $fields = explode(',', $fields);
                // }
                foreach($fields as $k=>$v){
                    $preg = $this->preg[$v];
                    if(array_key_exists($v, $data)){
                        if($bool){
                            self::setTypeMsg('regex', $this->getErrorMsg('regex', $v));
                            if(strpos($preg, '|') !== false){
                                $rule[strtolower($v)] = ['regex'=>$preg];
                            }else{
                                $rule[strtolower($v)] = 'regex:'.$preg;
                            }
                        }else{
                            unset($data[$v]);
                        }
                    }else{
                        if($bool){
                            unset($data[$v]);
                        }else{
                            self::setTypeMsg('regex', $this->getErrorMsg('regex', $v));
                            if(strpos($preg, '|') !== false){
                                $rule[] = [strtolower($v)=>['regex'=>$preg]];
                            }else{
                                $rule[] = [strtolower($v)=>'regex:'.$preg];
                            }
                        }
                    }
                }
            }else{
                throw new Exception('Missing parameter');
            }
            $this->rule = $rule;
            if(false == $this->check($data)){
                $this->flag = false;
            }
        }
        return $this;
    }

    /**
     **验证两个字段的值是否相等
     * @param  [arr]            $data       需要验证的数组
     * @param  [arr]            $fields     要比较的两个字段
     * @return [type]                       [description]
     */
    public function checkEqual(array $data=[], $fields=[])
    {
        if($this->flag){
            if(empty($data) && empty($fields)){
                throw new Exception('Missing parameter');
            }
            list($data, $fields) = $this->setData($data, $fields, true);
            $rule[$fields[0]] = 'confirm:'.$fields[1];
            $this->rule = $rule;
            self::setTypeMsg('confirm','确认密码输入错误');
            if(false == $this->check($data)){
                $this->flag = false;
            }
        }
        return $this;
    }

    //链式终结
    public function end()
    {
        return $this->flag;
    }

}