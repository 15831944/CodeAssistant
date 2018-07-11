<?php
/**
 * Created by PhpStorm.
 * User: Shadow
 * Date: 2018/3/9
 * Time: 16:30
 */

namespace app\index\controller;

// 引入控制器
use app\index\model\Group;
use app\index\model\Project;
use app\index\model\Version;
use think\Controller;

// 引入视图
use think\View;

// 引入实体对象
use app\index\model\User;

// 引入验证对象
use app\common\validate\BaseValidate;

class Account extends Controller
{
    /**
     * @return string
     * @throws \think\Exception
     */
    public function index()
    {
        $view = new View();
        return $view->fetch();
    }

    // 登录验证
    /**
     * @throws \think\exception\DbException
     */
    public function login()
    {
        // 得到 post 数据
        $params = $_POST; //$params=input('param.');
        $name   = input("post.username");
        $pwd    = input("post.password");

        // 验证对象
        $validate = new BaseValidate();
        $check = $validate->checkRequire($params)->end();

        // 验证信息
        if (!$check)
        {
            //$this->error($validate->getError());
            echo $validate->getError();
        }
        else
        {
            // 用户信息检测
            $user = User::get([
                'UserName' => $name,
                'Password' => $pwd
            ]);

            // 如果数据库中的数据信息匹配
            if ($user)
            {
                echo $user['UserId'];

                // 赋值（当前作用域）
                //Session::set('UserInfo', $user);

                // 记录操作, 并跳转页面.
                //$this->success("登录成功", "admin");
                //echo "登录成功";
                //$this->redirect("/hplus/index.html");
            }
            else
            {
                $email = User::get([
                    'EMail' => $name,
                    'Password' => $pwd
                ]);

                if ($email)
                {
                    echo "success";
                }
                else
                {
                    echo "请检查用户名或密码是否正确!!!";
                }
                //$this->error('登陆失败, 请检查用户名或密码是否正确!!!');
                //echo "请检查用户名或密码是否正确!!!";
            }
        }
    }


    /**
     * @throws \think\Exception
     */
    public function register()
    {
        // 得到 post 数据
        $params  = $_POST; //$params=input('param.');
        $name    = input("post.username");
        $pwd     = input("post.password");
        $confirm = input("post.confirm");
        $email   = input("post.email");

        // 验证对象
        $validate = new BaseValidate();
        $check = $validate->checkRequire($params)->checkEqual($params,['password','confirm'])->end();

        // 验证信息
        if (!$check)
        {
            //$this->error($validate->getError());
            echo $validate->getError();
        }
        else
        {
            $user = new User();
            $flag = $user->save([
                'UserName' => $name,
                'EMail'    => $email,
                'Password' => $pwd
            ]);

            if($flag)
            {
                echo "success";
            }
            else
            {
                echo "error";
            }
        }


    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // 网络数据同步

    private function loopFun( $dir )
    {
        //取出文件或者文件夹
        $list = scandir( $dir );

        foreach( $list as $file )
        {
            $location_dir = $dir . '/' . $file;

            echo $location_dir . ';';

            //判断是否是文件夹 是就调用自身函数再去进行处理
            if( is_dir( $location_dir ) && '.' != $file && '..' != $file )
            {
                $this->loopFun( $location_dir );
            }
        }
    }


    public function GetFileInfo()
    {
        /*$file1 = glob('*.php');  //匹配所有php文件
        print_r($file1);*/

        $id      = input("post.id");
        $type     = input("post.type");

        if($type == 1)
        {
            $dirPath = 'cloud/'.$id.'/Code';

            if (!file_exists('cloud/'.$id)){
                mkdir('cloud/'.$id);
            }

            if (!file_exists('cloud/'.$id.'/Code')){
                mkdir('cloud/'.$id.'/Code');
            }

            //调用处理函数
            $this->loopFun( $dirPath );
        }
        else if($type == 2)
        {
            $dirPath = 'cloud/'.$id.'/File';

            if (!file_exists('cloud/'.$id)){
                mkdir('cloud/'.$id);
            }

            if (!file_exists('cloud/'.$id.'/File')){
                mkdir('cloud/'.$id.'/File');
            }

            //调用处理函数
            $this->loopFun( $dirPath );
        }
    }


	public function GetProjectInfo()
    {
        $id      = input("post.id");

        $dirPath = 'cloud/'.$id.'/Project';

        if (!file_exists('cloud/'.$id)){
            mkdir('cloud/'.$id);
        }

        if (!file_exists('cloud/'.$id.'/Project')){
            mkdir('cloud/'.$id.'/Project');
        }


		//取出文件或者文件夹
        $list = scandir( $dirPath );

        foreach( $list as $file )
        {
            $location_dir = $dirPath . '/' . $file;
            echo $location_dir . ';';
        }
    }

	
    public function UpLoadFile()
    {
        // 获取表单上传文件 例如上传了001.jpg
        $file = request()->file('file');

        $path    = input("param.path");
        $target  = input("param.target");

        // 移动到框架应用根目录/public/uploads/ 目录下
        if($file){
            //$info = $file->move(ROOT_PATH . 'public' . DS . 'cloud');
            $info = $file->move('./upload/');

            if($info){
                // 成功上传后 获取上传信息
                // 输出 jpg
                //echo $info->getExtension();
                // 输出 20160820/42a79759f284b767dfcb2a0197904287.jpg
                echo $info->getSaveName();
                // 输出 42a79759f284b767dfcb2a0197904287.jpg
                //echo $info->getFilename();

                // 移动目标
                //rename($info->getSaveName(), '/1234554321.rtf');
                //exec("mv $info->getSaveName()  3213.rtf");

                /*if (!file_exists('cloud/'.$path))
                {
                    mkdir('cloud/'.$path);
                }*/

                //copy($_SERVER['DOCUMENT_ROOT'].'/upload/'.$info->getSaveName(),"./cloud/".'123.rtf');
                //unlink($_SERVER['DOCUMENT_ROOT'].'/upload/'.$info->getSaveName());
            }else{
                // 上传失败获取错误信息
                echo $file->getError();
            }
        }
    }


    public function ReMoveFile()
    {
        $path    = input("param.path");
        $target  = input("param.target");
        $name    = input("param.name");
        $id      = input("param.id");
        $parent  = input("param.parent");
        $sub     = input("param.sub");
        $item    = input("param.item");

        if (!file_exists('cloud/'.$id.'/'.$parent))
        {
            mkdir('cloud/'.$id.'/'.$parent);
        }

        if (!file_exists('cloud/'.$id.'/'.$parent.'/'.$sub))
        {
            mkdir('cloud/'.$id.'/'.$parent.'/'.$sub);
        }

        if (!file_exists('cloud/'.$id.'/'.$parent.'/'.$sub.'/'.$item))
        {
            mkdir('cloud/'.$id.'/'.$parent.'/'.$sub.'/'.$item);
        }

        // 重命名
        //copy($_SERVER['DOCUMENT_ROOT'].'/upload/'.$path,"./cloud/".$target.'/'.$name);
        rename($_SERVER['DOCUMENT_ROOT'].'/upload/'.$path, "./cloud/".$target.'/'.$name);

        // 清除缓存
        $string_arr = explode("\\", $path );
        $remove = $string_arr[0];
        $this->del_dir($_SERVER['DOCUMENT_ROOT'].'/upload/'.$remove);

        // 返回
        echo "Success";
    }
	
	
	public function GetFileModifyTime()
    {
        $path    = input("post.path");
        $dirPath = 'cloud/'.$path;

        $a = filemtime($dirPath);
        echo date("Y-m-d H:i:s", $a);
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // 网络项目管理

    // 得到用户数据
    public function GetUserInfo()
    {
        $user = new User();
        return json_encode($user->paginate(100000));
    }


    // 得到小组数据
    public function GetGroup()
    {
        $project_id = input('id');

        $group = new Group();
        return json_encode($group->where(['ProjectId' => $project_id])->paginate(100000));
    }


    // 得到项目数据
    public function ProjectInfo()
    {
        $id = input('id');
        $share = input('share');

        $project = new Project();
        if ($share == 2)
            return json_encode($project->where(['UserId' => $id])->paginate(100000));
        else if($share == 1)
        {
            return json_encode($project->where(['ShareStatus' => 1])->paginate(100000));
        }
        else
        {
            $group = new Group();
            $name  = User::get($id)['UserName'];
            $list  = $group->where("Member LIKE '%$name%'")->select();

            $ids     = "(";
            $count   = $group->where("Member LIKE '%$name%'")->count();
            if(!$count)
                return json_encode($group->where(['ProjectId' => 0])->paginate());

            for($i=0; $i<$count; $i++)
            {
                if($i == 0)
                    $ids = $ids . $list[$i]->ProjectId;
                else
                    $ids = $ids . "," . $list[$i]->ProjectId;
            }
            $ids = $ids .")";

            return json_encode($project->where("ProjectId IN $ids")->paginate(100000));
        }
    }


    // 得到版本数据
    public function GetVersionInfo()
    {
        $project = input('project_id');

        $version = new Version();
        return json_encode($version->where(['ProjectId' => $project])->paginate(100000));
    }


    // 获得项目数据
    public function GetProject()
    {
        // 获取参数
        $user_id = input("id");

        $project = new Project();
        return json_encode($project->where(['UserId' => $user_id])->paginate(100000));
    }


    public function GetOpenProject()
    {
        $project = new Project();
        return json_encode($project->where(['ShareStatus' => 1])->paginate(100000));
    }


    /**
     * @throws \think\db\exception\DataNotFoundException
     * @throws \think\db\exception\ModelNotFoundException
     * @throws \think\exception\DbException
     */
    public function GetShareProject()
    {
        $user_id = input("id");
        $project = new Project();
        $group   = new Group();

        $name    = User::get($user_id)['UserName'];
        $list    = $group->where("Member LIKE '%$name%'")->select();

        $ids     = "(";
        $count   = $group->where("Member LIKE '%$name%'")->count();
        if(!$count)
            return json_encode($group->where(['ProjectId' => 0])->paginate());

        for($i=0; $i<$count; $i++)
        {
            if($i == 0)
                $ids = $ids . $list[$i]->ProjectId;
            else
                $ids = $ids . "," . $list[$i]->ProjectId;
        }
        $ids = $ids .")";

        return json_encode($project->where("ProjectId IN $ids")->paginate(100000)->each(
            function($item, $key)
            {
                $group   = new Group();
                $role    = $group->where(['ProjectId' => $item['ProjectId']])->select()[0]['Role'];
                $member  = $group->where(['ProjectId' => $item['ProjectId']])->select()[0]['Member'];

                $roles   = explode(";", $role);
                $members = explode(";", $member);
                $name    = User::get(input("id"))['UserName'];

                $member_role = $roles[array_search($name, $members)];

                $item['Role'] = $member_role; //给数据集追加字段Role并赋值
                return $item;
            }
        ));
    }


    // 创建项目
    /**
     * @throws \think\exception\DbException
     */
    public function CreateProject()
    {
        // 获取参数
        $user_id = input("id");
        $name    = input("project");
        $explain = input("explain");
        $share   = input("share");
        $time    = date("Y-m-d H:i:s");
        $member  = input("member");
        $role    = input("role");
        $type    = input("type");

        $project = new Project();
        if($share != 2)
        {
            // 检测私人项目
            if($share == 0)
            {
                if($project->where(['UserId' => $user_id, 'ProjectName' => $name])->count())
                {
                    echo "used";
                    return;
                }
            }
            else
            {
                // 检测公共项目
                if($project->where(['ProjectName' => $name, 'ShareStatus' =>  1])->count())
                {
                    echo "shared";
                    return;
                }
            }

            if($project->isUpdate(false)->save(['UserId' => $user_id, 'ProjectName' => $name, 'ProjectExplain' => $explain, 'CreateTime' => $time, 'ShareStatus' => $share]))
            {
                // 创建一个最初的 master 版本
                $version = new Version();
                $editor  = User::get($user_id)['UserName'];
                $number  = $project->ProjectId;

                if($version->isUpdate(false)->save(['ProjectId' => $number, 'Remark' => '最初的版本', 'ModifyTime' => $time, 'Version' => 'Default', 'Branch' => 'master', 'Editor' => $editor]))
                    // 返回项目编号
                    return $number;
                else
                    return "error";
            }
            else
            {
                echo "error";
            }
        }
        else
        {
            // 检测公共项目
            if($project->where(['ProjectName' => $name, 'ShareStatus' => 2])->count())
                echo "shared";
            else
            {
                if($project->isUpdate(false)->save(['UserId' => $user_id, 'ProjectName' => $name, 'ProjectExplain' => $explain, 'CreateTime' => $time, 'ShareStatus' => $share]))
                {
                    $group = new Group();
                    $project_id = $project->ProjectId;

                    $user_name  = User::get($user_id)['UserName'];
                    $member     = $user_name.";".$member;
                    $role       = "2".";".$role;
                    $type       = "all".";".$type;

                    if($group->isUpdate(false)->save(['Member' => $member, 'Role' => $role, 'Type' => $type, 'ProjectId' => $project_id]))
                    {
                        // 创建一个最初的 master 版本
                        $version = new Version();
                        $editor  = User::get($user_id)['UserName'];

                        if($version->isUpdate(false)->save(['ProjectId' => $project_id, 'Remark' => '最初的版本', 'ModifyTime' => $time, 'Version' => 'Default', 'Branch' => 'master', 'Editor' => $editor]))
                            // 返回项目编号
                            echo $project_id;
                        else
                            return "error";
                    }
                    else
                        echo "error";
                }
                else
                {
                    echo "error";
                }
            }
        }
    }


    // 编辑项目
    /**
     * @return string
     * @throws \think\exception\DbException
     */
    public function EditProject()
    {
        // 获取参数
        $project_id = input("project_id");
        $name    = input("project");
        $explain = input("explain");
        $share   = input("share");
        $time    = date("Y-m-d H:i:s");
        $member  = input("member");
        $role    = input("role");
        $type    = input("type");

        $user_id = Project::get($project_id)['UserId'];
        $old_name= Project::get($project_id)['ProjectName'];

        $project = new Project();
        if(!$project->where(['ProjectId' => $project_id])->count())
            return "NotFind";

        if($name != $old_name)
        {
            // 重命名
            if(!rename("cloud/$user_id/Project/$old_name","cloud/$user_id/Project/$name"))
                return "error";
        }

        if($share != 2)
        {
            if($project->isUpdate()->save(['ProjectName' => $name, 'ProjectExplain' => $explain, 'ModifyTime' => $time, 'ShareStatus' => $share], ['ProjectId' => $project_id]))
            {
                return "success";
            }
            else
                return "NotChange";
        }
        else
        {
            if($project->isUpdate()->save(['ProjectName' => $name, 'ProjectExplain' => $explain, 'ModifyTime' => $time, 'ShareStatus' => $share], ['ProjectId' => $project_id]))
            {
                $group = new Group();
                if($group->where(['ProjectId' => $project_id])->count())
                {
                    $group->isUpdate()->save(['Member' => $member, 'Role' => $role, 'Type' => $type] ,['ProjectId' => $project_id]);
                        return "success";
                }
                else
                {
                    if($group->isUpdate(false)->save(['Member' => $member, 'Role' => $role, 'Type' => $type, 'ProjectId' => $project_id]))
                        return "success";
                }
            }
            else
                return "NotChange";
        }

        return "error";
    }


    // 遍历删除目录和目录下所有文件
    function del_dir($dir)
    {
        if (!is_dir($dir))
        {
            return false;
        }
        $handle = opendir($dir);
        while (($file = readdir($handle)) !== false)
        {
            if ($file != "." && $file != "..")
            {
                is_dir("$dir/$file") ? $this->del_dir("$dir/$file") : @unlink("$dir/$file");
            }
        }
        if (readdir($handle) == false)
        {
            closedir($handle);
            @rmdir($dir);
        }

        return true;
    }


    // 删除项目
    /**
     * @return string
     * @throws \think\db\exception\DataNotFoundException
     * @throws \think\db\exception\ModelNotFoundException
     * @throws \think\exception\DbException
     */
    public function DeleteProject()
    {
        $project_id = input('project_id');
        $project    = new Project();

        if($project->where(['ProjectId' => $project_id])->count())
        {
            // 得到共享状态
            $share = $project->where(['ProjectId' => $project_id])->select()[0]['ShareStatus'];
            $user  = $project->where(['ProjectId' => $project_id])->select()[0]['UserId'];
            $name  = $project->where(['ProjectId' => $project_id])->select()[0]['ProjectName'];

            //  删除项目
            if($project->where(['ProjectId' => $project_id])->delete())
            {
                //  删除所有版本和组数据
                $version = new Version();
                $group   = new Group();

                if($share != 2)
                {
                    if( $this->del_dir("cloud/".$user."/Project/".$name) && $version->where(['ProjectId' => $project_id])->delete() )
                        // 返回删除成功
                        return "success";
                }
                else
                {
                    if( $version->where(['ProjectId' => $project_id])->delete() && $group->where(['ProjectId' => $project_id])->delete() && $this->del_dir("cloud/".$user."/Project/".$name))
                        // 返回删除成功
                        return "success";
                }
            }
            else
                return "error";
        }
        else
            return "error";
    }


    // 获取版本
    /**
     * @return string
     * @throws \think\exception\DbException
     */
    public function GetVersion()
    {
        $project_id = input('project_id');
        $user_id    = input('user_id');

        $version = new Version();
        if($version->where(['ProjectId' => $project_id])->count())
        {
            return json_encode($version->where(['ProjectId' => $project_id])->paginate(100000));
        }
        else
            return json_encode($version->where(['ProjectId' => 0])->paginate(1));
    }


    // 增加版本
    /**
     * @return mixed|string
     * @throws \think\exception\DbException
     */
    public function AddVersion()
    {
        $user_id = input('id');
        $project = input('project_id');
        $explain = input('explain');
        $version = input('version');
        $branch  = input('branch');
        $editor  = User::get($user_id)['UserName'];

        $time    = date("Y-m-d H:i:s");

        $query = new Version();
        if($query->where(['ProjectId' => $project, 'Version' => $version, 'Branch' => $branch])->count())
            return "used";
        else
        {
            if($query->where(['ProjectId' => $project, 'Branch' => 'master'])->count() && $branch == "master")
                return "master";
            if($query->where(['ProjectId' => $project, 'Branch' => 'develop'])->count() && $branch == "develop")
                return "develop";

            if($query->isUpdate(false)->save(['ProjectId' => $project, 'Remark' => $explain, 'ModifyTime' => $time, 'Version' => $version, 'Branch' => $branch, 'Editor' => $editor]))
            {
                $version_id = $query->VersionId;
                return $version_id;
            }
            else
            {
                echo "error";
            }
        }
    }


    // 合并版本
    /**
     * @return string
     * @throws \think\exception\DbException
     */
    public function MergeVersion()
    {
        $id      = input('version_id');
        $branch  = input('branch');
        $project = input('project');
        $version = input('version');
        $user    = input('user');
        $target  = "cloud/$user/Project/$project/$branch/$version.7z";

        $query = new Version();
        if($query->where(['VersionId' => $id])->count())
        {
            //  得到原数据
            $old_version = Version::get($id);
            $new_branch  = "";

            //  判断分支
            if($branch == "release")
            {
                $new_branch = "master";
            }
            /*else if($branch == "develop")
            {
                $new_branch = "release";
            }*/
            else if($branch == "feature")
            {
                $new_branch = "develop";
            }
            else if($branch == "hotfix")
            {
                $new_branch = "master";
            }
            else
                // 分支无法合并
                return "top";

            //  更新数据
            $new_version = $query->where(['Branch' => $new_branch])->select()[0];
            if(!$query->isUpdate()->save(['Remark'=> $old_version['Remark'], 'ModifyTime' => $old_version['ModifyTime'], 'Version' => $old_version['Version'], 'Branch' => $new_branch, 'Editor' => $old_version['Editor']], ['Branch' => $new_branch]))
                return "error";

            //  删除原数据
            if(!$query->where(['VersionId' => $id])->delete())
                return "error";

            //  合并数据
            $new_target  = "cloud/$user/Project/$project/$new_branch/$version.7z";
            rename($target, $new_target);

            $old_branch  = $new_version['Branch'];
            $old_file    = $new_version['Version'];
            $old_version = "cloud/$user/Project/$project/$old_branch/$old_file.7z";
            unlink($old_version);

            return "success";
        }
        else
            return "NotFind";
    }


    // 编辑版本
    /**
     * @return string
     * @throws \think\exception\DbException
     */
    public function ModifyVersion()
    {
        $user_id = input('id');
        $project = input('project_id');
        $explain = input('explain');
        $version = input('version');
        $branch  = input('branch');
        $number  = input('version_id');
        $editor  = User::get($user_id)['UserName'];

        $time    = date("Y-m-d H:i:s");

        $query = new Version();
        $old_name   = Version::get($number)['Version'];
        $name       = Project::get($project)['ProjectName'];
        $old_branch = Version::get($number)['Branch'];

        if($old_name != $version && $query->where(['Version' => $version, 'Branch' => $branch])->count())
            return "used";
        else
        {
            if($query->where(['ProjectId' => $project, 'Branch' => 'master'])->count() && $branch == "master" && $old_branch != "master")
                return "master";
            if($query->where(['ProjectId' => $project, 'Branch' => 'develop'])->count() && $branch == "develop" && $old_branch != "develop")
                return "develop";

            if($old_branch == "master" && $branch != "master")
                return "default";

            if($version != $old_name || $branch != $old_branch)
            {
                if($branch != $old_branch)
                {
                    if (!file_exists('cloud/'.$user_id.'/'.'Project'.'/'.$name.'/'.$branch))
                    {
                        mkdir('cloud/'.$user_id.'/'.'Project'.'/'.$name.'/'.$branch);
                    }
                }

                // 重命名
                if(!rename("cloud/$user_id/Project/$name/$old_branch/$old_name.7z","cloud/$user_id/Project/$name/$branch/$version.7z"))
                    return "error";
            }

            if($query->isUpdate()->save(['ProjectId' => $project, 'Remark'=>$explain, 'ModifyTime' => $time, 'Version' => $version, 'Branch' => $branch, 'Editor' => $editor], ['VersionId' => $number]))
            {
                return "success";
            }
            else
            {
                return "error";
            }
        }
    }


    // 移除版本
    public function RemoveVersion()
    {
        $id      = input('version_id');
        $branch  = input('branch');
        $project = input('project');
        $version = input('version');
        $user    = input('user');
        $target  = "cloud/$user/Project/$project/$branch/$version.7z";

        $query = new Version();
        if($query->where(['VersionId' => $id])->count())
        {
            if(unlink($target) && $query->where(['VersionId' => $id])->delete())
                return "success";
            else
                return "error";
        }
        else
            return "NotFind";
    }

}