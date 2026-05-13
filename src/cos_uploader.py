# -*- coding: utf-8 -*-
import sys
import os
from qcloud_cos import CosConfig
from qcloud_cos import CosS3Client

# ============================================================
# 配置信息（根据你的实际情况填写）
# ============================================================
secret_id = 'AKIDc8SXO0NVP4UnuoW1HNGvMcgV6Tq6JPM6'
secret_key = 'YrwmV2NUQAfwyFIAvUotkMktnX6Vs2sX'
region = 'ap-guangzhou'  # 广州地域
bucket = 'zerogravity-data-1331540454' # 你的桶名称

def upload_file(local_path):
    config = CosConfig(Region=region, SecretId=secret_id, SecretKey=secret_key)
    client = CosS3Client(config)

    # 提取文件名作为云端路径
    file_name = os.path.basename(local_path)
    # 存放在 avatars 目录下
    cos_path = 'avatars/' + file_name

    try:
        response = client.upload_file(
            Bucket=bucket,
            LocalFilePath=local_path,
            Key=cos_path,
            PartSize=1,
            MAXThread=5,
            EnableMD5=False
        )
        # 生成访问链接
        url = f"https://{bucket}.cos.{region}.myqcloud.com/{cos_path}"
        return url
    except Exception as e:
        print(f"Error: {str(e)}")
        return None

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 cos_uploader.py <local_file_path>")
        sys.exit(1)
    
    local_file = sys.argv[1]
    result_url = upload_file(local_file)
    if result_url:
        print(result_url)
    else:
        sys.exit(1)
