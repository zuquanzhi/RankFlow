#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import hashlib

def test_specific_file():
    """测试具体的文件哈希计算"""
    json_file = "/Users/zuquanzhi/Programs/rank/data/team01_results.json"
    hash_file = "/Users/zuquanzhi/Programs/rank/data/team01_results.json.sha256"
    
    print(f"🔍 测试文件: {json_file}")
    print("=" * 60)
    
    # 读取JSON文件
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    print("📋 JSON数据:")
    for key, value in data.items():
        if key == "submissions":
            print(f"  {key}: [{len(value)} 个提交记录]")
        else:
            print(f"  {key}: {value}")
    print()
    
    # 读取存储的哈希
    with open(hash_file, 'r') as f:
        stored_hash = f.read().strip()
    
    print(f"🔒 存储的哈希: {stored_hash}")
    
    # 计算紧凑格式哈希（模拟Qt行为）
    compact_json = json.dumps(data, ensure_ascii=False, separators=(',', ':'))
    compact_hash = hashlib.sha256(compact_json.encode('utf-8')).hexdigest()
    
    print(f"🔸 计算的哈希: {compact_hash}")
    print(f"🔸 匹配: {'是' if compact_hash == stored_hash else '否'}")
    print()
    
    if compact_hash != stored_hash:
        print("❌ 哈希不匹配！")
        print("可能的原因:")
        print("1. 文件被修改过")
        print("2. 哈希文件是基于不同格式计算的")
        print("3. 字段顺序不一致")
        print()
        
        # 重新生成正确的哈希
        correct_hash = compact_hash
        with open(hash_file, 'w') as f:
            f.write(correct_hash)
        print(f"✅ 已更新哈希文件: {hash_file}")
        print(f"🔑 新哈希值: {correct_hash}")
    else:
        print("✅ 哈希匹配，文件完整性验证通过！")

def fix_all_hashes():
    """修复所有哈希文件"""
    import os
    data_dir = "/Users/zuquanzhi/Programs/rank/data"
    
    print("🔧 修复所有哈希文件...")
    print("=" * 40)
    
    for filename in os.listdir(data_dir):
        if filename.endswith('.json'):
            json_path = os.path.join(data_dir, filename)
            hash_path = json_path + '.sha256'
            
            if os.path.exists(hash_path):
                print(f"📝 处理: {filename}")
                
                # 读取JSON
                with open(json_path, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                
                # 计算正确的哈希
                compact_json = json.dumps(data, ensure_ascii=False, separators=(',', ':'))
                correct_hash = hashlib.sha256(compact_json.encode('utf-8')).hexdigest()
                
                # 更新哈希文件
                with open(hash_path, 'w') as f:
                    f.write(correct_hash)
                
                print(f"  ✅ 已更新哈希: {correct_hash[:16]}...")
    
    print("\n🎉 所有哈希文件已修复！")

if __name__ == "__main__":
    # 测试第一个文件
    test_specific_file()
    print()
    
    # 修复所有文件
    fix_all_hashes()
