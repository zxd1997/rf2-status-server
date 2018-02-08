rf2游戏服务器状态HTTP查询插件
=======================

[![Build status](https://ci.appveyor.com/api/projects/status/qcn7hx2a4e7ml0qv?svg=true)](https://ci.appveyor.com/project/pangliang/rf2-status-server)

1. 获取ScoringInfo
> http://ip:port/getScoringInfo

2. 获取GraphicsInfo, 主要是当前在观看谁
> http://ip:port/getGraphicsInfo

3. 设置camera, 设置游戏观看哪位车手
> http://ip:port/cameraControl?mID=车手id&mCameraType=视角类型
