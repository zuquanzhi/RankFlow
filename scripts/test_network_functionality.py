#!/usr/bin/env python3
"""
网络功能测试脚本
测试竞赛排行榜系统的网络功能是否正常工作
"""

import requests
import json
import time
import sys

def test_server_endpoints():
    """测试服务器所有端点"""
    base_url = "http://localhost:8080"
    endpoints = [
        "/api/ping",
        "/api/teams", 
        "/api/contest",
        "/api/problems"
    ]
    
    print("🔍 测试服务器端点...")
    all_passed = True
    
    for endpoint in endpoints:
        try:
            url = base_url + endpoint
            response = requests.get(url, timeout=5)
            
            if response.status_code == 200:
                print(f"✅ {endpoint} - 状态码: {response.status_code}")
                
                # 如果是JSON响应，验证格式
                if 'application/json' in response.headers.get('content-type', ''):
                    data = response.json()
                    if endpoint == "/api/teams":
                        validate_teams_format(data)
                    elif endpoint == "/api/contest":
                        validate_contest_format(data)
                    elif endpoint == "/api/problems":
                        validate_problems_format(data)
            else:
                print(f"❌ {endpoint} - 状态码: {response.status_code}")
                all_passed = False
                
        except requests.exceptions.RequestException as e:
            print(f"❌ {endpoint} - 请求失败: {e}")
            all_passed = False
            
    return all_passed

def validate_teams_format(data):
    """验证队伍数据格式"""
    if 'teams' not in data:
        print("❌ 队伍数据缺少 'teams' 字段")
        return False
        
    teams = data['teams']
    if not teams:
        print("❌ 队伍列表为空")
        return False
        
    # 检查第一个队伍的格式
    team = teams[0]
    required_fields = ['team_id', 'team_name', 'total_score', 'last_submit_time', 'submissions']
    
    for field in required_fields:
        if field not in team:
            print(f"❌ 队伍数据缺少必需字段: {field}")
            return False
            
    print(f"✅ 队伍数据格式正确 (共{len(teams)}支队伍)")
    return True

def validate_contest_format(data):
    """验证竞赛信息格式"""
    required_fields = ['contest_id', 'contest_name', 'start_time', 'duration']
    
    for field in required_fields:
        if field not in data:
            print(f"❌ 竞赛数据缺少必需字段: {field}")
            return False
            
    print("✅ 竞赛信息格式正确")
    return True

def validate_problems_format(data):
    """验证题目列表格式"""
    if 'problems' not in data:
        print("❌ 题目数据缺少 'problems' 字段")
        return False
        
    problems = data['problems']
    if not problems:
        print("❌ 题目列表为空")
        return False
        
    # 检查第一个题目的格式
    problem = problems[0]
    required_fields = ['problem_id', 'problem_name', 'color']
    
    for field in required_fields:
        if field not in problem:
            print(f"❌ 题目数据缺少必需字段: {field}")
            return False
            
    print(f"✅ 题目数据格式正确 (共{len(problems)}道题目)")
    return True

def test_data_updates():
    """测试数据实时更新"""
    print("\n🔄 测试数据实时更新...")
    
    try:
        # 获取第一次数据
        response1 = requests.get("http://localhost:8080/api/teams", timeout=5)
        data1 = response1.json()
        timestamp1 = data1.get('timestamp')
        
        print(f"第一次获取: {timestamp1}")
        
        # 等待一段时间
        print("等待15秒以测试数据更新...")
        time.sleep(15)
        
        # 获取第二次数据
        response2 = requests.get("http://localhost:8080/api/teams", timeout=5)
        data2 = response2.json()
        timestamp2 = data2.get('timestamp')
        
        print(f"第二次获取: {timestamp2}")
        
        if timestamp1 != timestamp2:
            print("✅ 数据正在实时更新")
            return True
        else:
            print("❌ 数据未更新")
            return False
            
    except Exception as e:
        print(f"❌ 测试数据更新失败: {e}")
        return False

def test_network_performance():
    """测试网络性能"""
    print("\n⚡ 测试网络性能...")
    
    response_times = []
    
    for i in range(5):
        try:
            start_time = time.time()
            response = requests.get("http://localhost:8080/api/teams", timeout=5)
            end_time = time.time()
            
            response_time = (end_time - start_time) * 1000  # 转换为毫秒
            response_times.append(response_time)
            
            print(f"请求 {i+1}: {response_time:.2f}ms")
            
        except Exception as e:
            print(f"❌ 请求 {i+1} 失败: {e}")
            
    if response_times:
        avg_time = sum(response_times) / len(response_times)
        print(f"✅ 平均响应时间: {avg_time:.2f}ms")
        
        if avg_time < 100:
            print("✅ 网络性能优秀")
        elif avg_time < 500:
            print("✅ 网络性能良好")
        else:
            print("⚠️  网络性能一般")
            
        return True
    else:
        print("❌ 所有网络请求都失败了")
        return False

def main():
    """主测试函数"""
    print("=" * 60)
    print("🚀 竞赛排行榜系统网络功能测试")
    print("=" * 60)
    
    # 检查服务器是否运行
    try:
        response = requests.get("http://localhost:8080/api/ping", timeout=2)
        print("✅ 测试服务器正在运行")
    except:
        print("❌ 测试服务器未运行，请先启动 test_server.py")
        sys.exit(1)
    
    # 运行所有测试
    tests = [
        ("端点测试", test_server_endpoints),
        ("数据更新测试", test_data_updates),
        ("性能测试", test_network_performance)
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"❌ {test_name} 执行失败: {e}")
            results.append((test_name, False))
    
    # 测试结果总结
    print(f"\n{'='*20} 测试结果总结 {'='*20}")
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "✅ 通过" if result else "❌ 失败"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
    
    print(f"\n总计: {passed}/{total} 项测试通过")
    
    if passed == total:
        print("🎉 所有测试通过！网络功能正常工作")
        return True
    else:
        print("⚠️  部分测试失败，请检查网络配置")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
