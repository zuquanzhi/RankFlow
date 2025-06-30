#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import hashlib

def debug_hash_calculation():
    """调试哈希计算过程"""
    json_file = "/Users/zuquanzhi/Programs/rank/data/test_alpha_results.json"
    hash_file = "/Users/zuquanzhi/Programs/rank/data/test_alpha_results.json.sha256"
    
    print("🔍 调试哈希计算过程")
    print("=" * 50)
    
    # 读取JSON文件
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # 读取存储的哈希
    with open(hash_file, 'r') as f:
        stored_hash = f.read().strip()
    
    print(f"📁 文件: {json_file}")
    print(f"🔒 存储的哈希: {stored_hash}")
    print()
    
    # 打印JSON数据结构
    print("📋 JSON数据结构:")
    for key, value in data.items():
        if key == "submissions":
            print(f"  {key}: [{len(value)} 个提交记录]")
            if len(value) > 0:
                print(f"    第一个提交: {value[0]}")
        else:
            print(f"  {key}: {value}")
    print()
    
    # 1. 计算紧凑格式哈希
    compact_json = json.dumps(data, ensure_ascii=False, separators=(',', ':'))
    compact_hash = hashlib.sha256(compact_json.encode('utf-8')).hexdigest()
    print(f"🔸 紧凑格式JSON长度: {len(compact_json)}")
    print(f"🔸 紧凑格式哈希: {compact_hash}")
    print(f"🔸 与存储哈希匹配: {'是' if compact_hash == stored_hash else '否'}")
    print()
    
    # 2. 尝试不同的排序
    sorted_data = dict(sorted(data.items()))
    sorted_json = json.dumps(sorted_data, ensure_ascii=False, separators=(',', ':'))
    sorted_hash = hashlib.sha256(sorted_json.encode('utf-8')).hexdigest()
    print(f"🔹 排序后JSON长度: {len(sorted_json)}")
    print(f"🔹 排序后哈希: {sorted_hash}")
    print(f"🔹 与存储哈希匹配: {'是' if sorted_hash == stored_hash else '否'}")
    print()
    
    # 3. 模拟Qt的JSON生成（按照TeamData::toJson的顺序）
    qt_like_data = {
        "team_id": data["team_id"],
        "team_name": data["team_name"], 
        "total_score": data["total_score"],
        "last_submit_time": data["last_submit_time"],
        "submissions": data["submissions"]
    }
    qt_json = json.dumps(qt_like_data, ensure_ascii=False, separators=(',', ':'))
    qt_hash = hashlib.sha256(qt_json.encode('utf-8')).hexdigest()
    print(f"🔺 Qt顺序JSON长度: {len(qt_json)}")
    print(f"🔺 Qt顺序哈希: {qt_hash}")
    print(f"🔺 与存储哈希匹配: {'是' if qt_hash == stored_hash else '否'}")
    print()
    
    # 4. 检查提交记录的字段顺序
    if "submissions" in data and len(data["submissions"]) > 0:
        submission = data["submissions"][0]
        print("📝 第一个提交记录字段:")
        for key, value in submission.items():
            print(f"  {key}: {value}")
        print()
        
        # 模拟Qt中Submission::toJson的顺序
        qt_submission = {
            "problem_id": submission["problem_id"],
            "timestamp": submission["timestamp"],
            "is_correct": submission["is_correct"],
            "run_time": submission["run_time"],
            "memory_usage": submission["memory_usage"]
        }
        print("🔧 Qt顺序的提交记录:")
        for key, value in qt_submission.items():
            print(f"  {key}: {value}")
    
    # 5. 显示紧凑JSON的前100个字符
    print(f"\n📄 紧凑JSON前100字符:")
    print(f"   {compact_json[:100]}...")

if __name__ == "__main__":
    debug_hash_calculation()
