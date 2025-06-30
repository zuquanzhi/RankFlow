# 测试数据使用指南

## 概述

本系统包含了完整的测试数据集，每个JSON数据文件都配有对应的SHA256哈希码文件，用于确保数据完整性和安全性。

## 数据文件结构

### 原始数据文件
- `team01_results.json` - `team07_results.json`: 原始团队数据
- `test_monitor.json`: 监控测试数据

### 新生成的测试数据
- `test_team_alpha.json`: Alpha测试队 (400分)
- `test_team_beta.json`: Beta测试团队 (500分)
- `test_team_gamma.json`: Gamma实验组 (600分)  
- `test_team_delta.json`: Delta高性能队 (700分)
- `test_team_epsilon.json`: Epsilon新手队 (200分)
- `test_zeta_results.json`: Zeta突击队 (500分)
- `test_eta_results.json`: Eta智能组 (600分)
- `test_theta_results.json`: Theta新星队 (100分)
- `test_iota_results.json`: Iota挑战者 (400分)
- `test_kappa_results.json`: Kappa精英团 (700分)

### 哈希码文件
每个JSON文件都有对应的`.sha256`文件，包含该文件的SHA256哈希码。

### 综合校验文件
- `data_checksums.sha256`: 包含所有JSON文件的SHA256校验和

## 数据格式

每个团队数据文件都包含以下字段：

```json
{
    "team_id": "团队ID",
    "team_name": "团队名称",
    "submissions": [
        {
            "problem_id": "题目ID (A, B, C, ...)",
            "timestamp": "提交时间 (ISO 8601格式)",
            "is_correct": "是否正确 (true/false)",
            "run_time": "运行时间 (毫秒)",
            "memory_usage": "内存使用 (字节)"
        }
    ],
    "total_score": "总分",
    "last_submit_time": "最后提交时间"
}
```

## 工具脚本

### 数据完整性校验
```bash
./scripts/verify_data_integrity.sh
```
- 验证所有数据文件的SHA256哈希码
- 检查文件是否被修改或损坏
- 显示文件统计信息

### 生成新的测试数据
```bash
./scripts/generate_test_data.sh
```
- 批量生成随机测试数据
- 自动计算并保存SHA256哈希码
- 更新综合校验文件

## 使用场景

### 1. 系统测试
使用这些数据文件测试排行榜系统的各项功能：
- 数据加载和解析
- 排序和查询
- 统计和分析
- 用户界面显示

### 2. 性能测试
利用不同规模的数据集测试系统性能：
- 小规模数据: test_theta (4题)
- 中等规模数据: test_alpha, test_beta (5-7题)
- 大规模数据: test_kappa, test_eta (7-8题)

### 3. 功能验证
验证二叉搜索树查询功能：
- 按分数排序查询
- 按时间范围筛选
- 多条件组合查询
- 统计分析功能

### 4. 数据完整性测试
使用哈希码验证数据传输和存储的完整性：
- 检测文件是否被意外修改
- 确保数据在不同环境间的一致性
- 防止数据损坏导致的系统错误

## 注意事项

1. **哈希码验证**: 在加载数据前建议先运行校验脚本
2. **文件编码**: 所有JSON文件使用UTF-8编码
3. **时间格式**: 使用ISO 8601格式 (YYYY-MM-DDTHH:MM:SS)
4. **数据一致性**: 修改数据文件后需要重新生成哈希码
5. **备份**: 建议定期备份原始数据文件

## 故障排除

### 哈希校验失败
如果校验失败，请检查：
1. 文件是否被意外修改
2. 文件编码是否正确
3. 是否有权限问题
4. 磁盘是否有错误

### 数据加载错误
如果系统无法加载数据：
1. 检查JSON格式是否正确
2. 验证文件路径是否正确  
3. 确认文件权限设置
4. 检查系统日志中的错误信息

## 扩展数据

如需添加更多测试数据：
1. 可以手动创建JSON文件
2. 使用`generate_test_data.sh`脚本批量生成
3. 计算并保存对应的SHA256哈希码
4. 更新`data_checksums.sha256`文件

## 技术支持

如有问题，请参考：
- 系统日志文件 (`logs/audit_*.log`)
- 技术文档 (`doc/technical/`)
- 用户手册 (`doc/user/`)
