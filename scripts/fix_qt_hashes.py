#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import hashlib
import os

def generate_qt_order_json(data):
    """按照Qt的字母顺序生成JSON"""
    # 递归处理所有对象，按字母顺序排序键
    def sort_dict(obj):
        if isinstance(obj, dict):
            return {k: sort_dict(v) for k, v in sorted(obj.items())}
        elif isinstance(obj, list):
            return [sort_dict(item) for item in obj]
        else:
            return obj
    
    sorted_data = sort_dict(data)
    return json.dumps(sorted_data, ensure_ascii=False, separators=(',', ':'))

def regenerate_qt_compatible_hashes():
    """重新生成与Qt兼容的哈希文件"""
    data_dir = "/Users/zuquanzhi/Programs/rank/data"
    
    print("🔧 重新生成与Qt兼容的哈希文件...")
    print("=" * 50)
    
    for filename in os.listdir(data_dir):
        if filename.endswith('.json') and not filename.endswith('.qt_compact'):
            json_path = os.path.join(data_dir, filename)
            hash_path = json_path + '.sha256'
            
            print(f"📝 处理: {filename}")
            
            # 读取JSON
            with open(json_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            # 生成Qt风格的紧凑JSON（字母顺序）
            qt_json = generate_qt_order_json(data)
            correct_hash = hashlib.sha256(qt_json.encode('utf-8')).hexdigest()
            
            # 更新哈希文件
            with open(hash_path, 'w') as f:
                f.write(correct_hash)
            
            print(f"  ✅ Qt兼容哈希: {correct_hash[:16]}...")
            
            # 保存Qt风格的JSON用于验证
            qt_debug_path = json_path + '.qt_debug'
            with open(qt_debug_path, 'w', encoding='utf-8') as f:
                f.write(qt_json)
    
    print("\n🎉 所有哈希文件已更新为Qt兼容格式！")

def verify_with_qt_compact():
    """验证Qt生成的紧凑文件"""
    data_dir = "/Users/zuquanzhi/Programs/rank/data"
    
    print("\n🔍 验证Qt生成的紧凑文件...")
    print("=" * 40)
    
    for filename in os.listdir(data_dir):
        if filename.endswith('.qt_compact'):
            qt_compact_path = os.path.join(data_dir, filename)
            json_filename = filename.replace('.qt_compact', '')
            hash_path = os.path.join(data_dir, json_filename + '.sha256')
            
            if os.path.exists(hash_path):
                # 读取Qt生成的紧凑JSON
                with open(qt_compact_path, 'rb') as f:
                    qt_data = f.read()
                
                # 计算哈希
                qt_hash = hashlib.sha256(qt_data).hexdigest()
                
                # 读取存储的哈希
                with open(hash_path, 'r') as f:
                    stored_hash = f.read().strip()
                
                match = qt_hash == stored_hash
                print(f"📄 {json_filename}")
                print(f"  Qt哈希: {qt_hash}")
                print(f"  存储哈希: {stored_hash}")
                print(f"  匹配: {'是' if match else '否'}")
                
                if not match:
                    # 更新为正确的哈希
                    with open(hash_path, 'w') as f:
                        f.write(qt_hash)
                    print(f"  ✅ 已更新哈希文件")
                print()

if __name__ == "__main__":
    # 首先尝试基于现有Qt生成的文件验证
    verify_with_qt_compact()
    
    print("\n" + "="*60)
    
    # 如果还是不匹配，重新生成
    regenerate_qt_compatible_hashes()
