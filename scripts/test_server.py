#!/usr/bin/env python3
"""
简单的竞赛数据测试服务器
用于演示网络数据传输功能

运行方式：
python3 test_server.py

访问：
http://localhost:8080/api/teams - 获取所有队伍数据
http://localhost:8080/api/contest - 获取竞赛信息
http://localhost:8080/api/problems - 获取题目列表
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import random
import time
from datetime import datetime, timedelta
import threading

class CompetitionData:
    def __init__(self):
        self.teams = []
        self.problems = ["A", "B", "C", "D", "E", "F", "G", "H"]
        self.contest_info = {
            "contest_id": "test_contest_2024",
            "contest_name": "测试编程竞赛",
            "start_time": "2024-07-07T09:00:00Z",
            "duration": "05:00:00",
            "freeze_time": "04:00:00"
        }
        self.generate_initial_data()
        
        # 启动数据更新线程
        self.update_thread = threading.Thread(target=self.update_data_periodically, daemon=True)
        self.update_thread.start()
    
    def generate_initial_data(self):
        """生成初始测试数据"""
        team_names = [
            "清华大学", "北京大学", "复旦大学", "上海交通大学", "浙江大学",
            "中国科学技术大学", "南京大学", "西安交通大学", "哈尔滨工业大学", "华中科技大学",
            "北京航空航天大学", "大连理工大学", "东南大学", "电子科技大学", "吉林大学"
        ]
        
        for i, name in enumerate(team_names):
            team = {
                "team_id": f"team{i+1:02d}",        # 使用team_id而不是id
                "team_name": name,                   # 使用team_name而不是name
                "total_score": random.randint(0, 800),  # 使用total_score而不是score
                "last_submit_time": (datetime.now() - timedelta(minutes=random.randint(1, 240))).isoformat() + "Z",
                "submissions": []                    # 使用submissions数组
            }
            
            # 生成提交记录而不是题目状态
            num_submissions = random.randint(0, 15)
            for _ in range(num_submissions):
                problem = random.choice(self.problems)
                submission = {
                    "problem_id": problem,
                    "timestamp": (datetime.now() - timedelta(minutes=random.randint(1, 200))).isoformat() + "Z",
                    "is_correct": random.random() < 0.7,  # 70% 概率正确
                    "run_time": random.randint(100, 5000),  # 运行时间(ms)
                    "memory_usage": random.randint(1024, 65536)  # 内存使用(bytes)
                }
                team["submissions"].append(submission)
            
            self.teams.append(team)
        
        # 按分数排序
        self.teams.sort(key=lambda x: x["total_score"], reverse=True)
    
    def update_data_periodically(self):
        """定期更新数据以模拟实时性"""
        while True:
            time.sleep(10)  # 每10秒更新一次
            self.update_random_team()
    
    def update_random_team(self):
        """随机更新一个队伍的数据"""
        if not self.teams:
            return
        
        team = random.choice(self.teams)
        
        # 可能增加新的提交
        if random.random() < 0.3:  # 30% 概率
            problem = random.choice(self.problems)
            new_submission = {
                "problem_id": problem,
                "timestamp": datetime.now().isoformat() + "Z",
                "is_correct": random.random() < 0.8,  # 80% 概率正确
                "run_time": random.randint(100, 5000),
                "memory_usage": random.randint(1024, 65536)
            }
            team["submissions"].append(new_submission)
            
            # 更新分数和最后提交时间
            if new_submission["is_correct"]:
                team["total_score"] += random.randint(10, 100)
            team["last_submit_time"] = new_submission["timestamp"]
        
        # 重新排序
        self.teams.sort(key=lambda x: x["total_score"], reverse=True)
        print(f"更新了队伍数据: {team['team_name']} -> {team['total_score']}分")

# 全局数据实例
competition_data = CompetitionData()

class CompetitionHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        """处理GET请求"""
        try:
            if self.path == "/api/teams":
                self.send_teams_data()
            elif self.path == "/api/contest":
                self.send_contest_info()
            elif self.path == "/api/problems":
                self.send_problems_list()
            elif self.path == "/api/ping":
                self.send_ping_response()
            elif self.path.startswith("/api/teams/"):
                team_id = self.path.split("/")[-1]
                self.send_team_data(team_id)
            else:
                self.send_error(404, "Not Found")
        except Exception as e:
            print(f"处理请求时出错: {e}")
            self.send_error(500, "Internal Server Error")
    
    def send_teams_data(self):
        """发送所有队伍数据"""
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        
        response = {
            "teams": competition_data.teams,
            "timestamp": datetime.now().isoformat() + "Z",
            "totalTeams": len(competition_data.teams)
        }
        
        self.wfile.write(json.dumps(response, ensure_ascii=False, indent=2).encode('utf-8'))
        print(f"发送了 {len(competition_data.teams)} 支队伍的数据")
    
    def send_team_data(self, team_id):
        """发送单个队伍数据"""
        team = next((t for t in competition_data.teams if t["id"] == team_id), None)
        
        if team:
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()
            self.wfile.write(json.dumps(team, ensure_ascii=False, indent=2).encode('utf-8'))
        else:
            self.send_error(404, f"Team {team_id} not found")
    
    def send_contest_info(self):
        """发送竞赛信息"""
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(json.dumps(competition_data.contest_info, ensure_ascii=False, indent=2).encode('utf-8'))
    
    def send_problems_list(self):
        """发送题目列表"""
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        
        colors = ["#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FECA57", "#FF9FF3", "#54A0FF", "#5F27CD"]
        problems_data = {
            "problems": [
                {
                    "problem_id": p, 
                    "problem_name": f"题目 {p}",
                    "color": colors[i % len(colors)]
                } 
                for i, p in enumerate(competition_data.problems)
            ]
        }
        self.wfile.write(json.dumps(problems_data, ensure_ascii=False, indent=2).encode('utf-8'))
    
    def send_ping_response(self):
        """发送ping响应"""
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        
        response = {
            "status": "ok",
            "timestamp": datetime.now().isoformat() + "Z",
            "server": "CompetitionTestServer/1.0"
        }
        self.wfile.write(json.dumps(response).encode('utf-8'))
    
    def log_message(self, format, *args):
        """自定义日志格式"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] {format % args}")

def run_server(port=8080):
    """启动测试服务器"""
    server_address = ('', port)
    httpd = HTTPServer(server_address, CompetitionHandler)
    
    print(f"🚀 竞赛数据测试服务器启动成功!")
    print(f"📡 监听端口: {port}")
    print(f"🌐 API endpoints:")
    print(f"   http://localhost:{port}/api/teams - 获取所有队伍数据")
    print(f"   http://localhost:{port}/api/contest - 获取竞赛信息")
    print(f"   http://localhost:{port}/api/problems - 获取题目列表")
    print(f"   http://localhost:{port}/api/ping - 连接测试")
    print(f"📊 数据会每10秒自动更新以模拟实时性")
    print(f"⏹️  按 Ctrl+C 停止服务器")
    print("-" * 50)
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n🛑 服务器已停止")
        httpd.server_close()

if __name__ == "__main__":
    import sys
    
    port = 8080
    if len(sys.argv) > 1:
        try:
            port = int(sys.argv[1])
        except ValueError:
            print("无效的端口号，使用默认端口 8080")
    
    run_server(port)
