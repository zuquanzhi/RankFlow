#!/bin/bash

# 重新生成基于紧凑JSON格式的哈希文件
# Regenerate hash files based on compact JSON format

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/../data"

echo "🔄 重新生成基于紧凑JSON格式的哈希文件..."
echo "Regenerating hash files based on compact JSON format..."
echo

cd "$DATA_DIR" || exit 1

# 临时Python脚本来计算紧凑JSON的哈希
cat > temp_hash_calc.py << 'EOF'
import json
import hashlib
import sys
import os

def calculate_compact_hash(json_file):
    """计算JSON文件紧凑格式的SHA256哈希"""
    try:
        with open(json_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # 生成紧凑格式的JSON（无缩进，无空格）
        compact_json = json.dumps(data, ensure_ascii=False, separators=(',', ':')).encode('utf-8')
        
        # 计算SHA256哈希
        hash_obj = hashlib.sha256(compact_json)
        return hash_obj.hexdigest()
    except Exception as e:
        print(f"错误处理 {json_file}: {e}")
        return None

def main():
    # 处理所有JSON文件
    for filename in os.listdir('.'):
        if filename.endswith('.json'):
            print(f"处理文件: {filename}")
            
            hash_value = calculate_compact_hash(filename)
            if hash_value:
                hash_file = filename + '.sha256'
                with open(hash_file, 'w') as f:
                    f.write(hash_value)
                print(f"  ✅ 已生成: {hash_file}")
                print(f"  🔒 哈希值: {hash_value}")
            else:
                print(f"  ❌ 失败: {filename}")
            print()

if __name__ == '__main__':
    main()
EOF

# 运行Python脚本
python3 temp_hash_calc.py

# 删除临时脚本
rm temp_hash_calc.py

echo "🔄 更新综合校验文件..."
echo "Updating comprehensive checksum file..."

# 重新生成综合校验文件（基于文件内容，不是紧凑格式）
shasum -a 256 *.json > data_checksums.sha256

echo "✅ 哈希文件重新生成完成！"
echo "Hash files regeneration completed!"
