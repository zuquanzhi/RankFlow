#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import hashlib
import os
import random
from datetime import datetime, timedelta

def generate_timestamp(base_hour=9, offset_hours=0):
    """生成随机时间戳"""
    base_date = datetime(2024, 7, 1, base_hour + offset_hours, 0, 0)
    random_minutes = random.randint(0, 59)
    random_seconds = random.randint(0, 59)
    return base_date.replace(minute=random_minutes, second=random_seconds).strftime("%Y-%m-%dT%H:%M:%S")

def generate_team_data(team_id, team_name, num_problems=5, success_rate=70):
    """生成团队数据"""
    submissions = []
    total_score = 0
    last_time = ""
    
    for i in range(num_problems):
        problem_id = chr(65 + i)  # A, B, C, ...
        attempts = random.randint(1, 3)
        
        for j in range(attempts):
            timestamp = generate_timestamp(8, i * 2 + j)
            is_correct = False
            run_time = random.randint(1000, 5000)
            memory_usage = random.randint(128000, 1024000)
            
            # 最后一次尝试有更高成功率
            if j == attempts - 1 and random.randint(1, 100) <= success_rate:
                is_correct = True
                run_time = random.randint(500, 3000)
                memory_usage = random.randint(64000, 512000)
                total_score += 100
            
            submission = {
                "problem_id": problem_id,
                "timestamp": timestamp,
                "is_correct": is_correct,
                "run_time": run_time,
                "memory_usage": memory_usage
            }
            submissions.append(submission)
            last_time = timestamp
    
    return {
        "team_id": team_id,
        "team_name": team_name,
        "submissions": submissions,
        "total_score": total_score,
        "last_submit_time": last_time
    }

def calculate_compact_hash(data):
    """计算紧凑JSON格式的SHA256哈希"""
    compact_json = json.dumps(data, ensure_ascii=False, separators=(',', ':')).encode('utf-8')
    return hashlib.sha256(compact_json).hexdigest()

def save_team_data(data, filename):
    """保存团队数据和哈希文件"""
    # 保存格式化的JSON文件（用于阅读）
    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)
    
    # 计算并保存紧凑格式的哈希
    hash_value = calculate_compact_hash(data)
    hash_filename = filename + '.sha256'
    with open(hash_filename, 'w') as f:
        f.write(hash_value)
    
    print(f"✅ 已生成: {filename}")
    print(f"🔒 哈希文件: {hash_filename}")
    print(f"📊 总分: {data['total_score']}")
    print()

def main():
    """主函数"""
    print("🚀 生成带正确哈希码的测试数据...")
    print()
    
    # 确保在data目录中
    if not os.path.exists('/Users/zuquanzhi/Programs/rank/data'):
        os.makedirs('/Users/zuquanzhi/Programs/rank/data')
    
    os.chdir('/Users/zuquanzhi/Programs/rank/data')
    
    # 定义测试团队
    teams = [
        ("test_alpha", "Alpha测试队", 5, 80),
        ("test_beta", "Beta测试团队", 6, 70), 
        ("test_gamma", "Gamma实验组", 7, 90),
        ("test_delta", "Delta高性能队", 6, 95),
        ("test_epsilon", "Epsilon新手队", 4, 50),
        ("test_zeta", "Zeta突击队", 5, 75),
        ("test_eta", "Eta智能组", 7, 85),
    ]
    
    # 生成团队数据
    for team_id, team_name, num_problems, success_rate in teams:
        data = generate_team_data(team_id, team_name, num_problems, success_rate)
        filename = f"{team_id}_results.json"
        save_team_data(data, filename)
    
    # 更新综合校验文件
    print("🔄 生成综合校验文件...")
    os.system("shasum -a 256 *.json > data_checksums.sha256")
    
    print("✅ 所有测试数据生成完成！")
    print("💡 这些数据文件现在应该能通过系统的哈希验证。")

if __name__ == "__main__":
    main()
