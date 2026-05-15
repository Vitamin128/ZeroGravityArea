import axios from 'axios';
// 1. 创建实例
const service = axios.create({
    // 根据你项目的情况，这里可以设置默认后端地址
    baseURL: 'http://192.168.52.131:8082',
    timeout: 10000, // 10秒超时
});
// 2. 请求拦截器：在发请求前做的事情
service.interceptors.request.use(
    (config) => {
        // 自动从本地存储获取 Token 并塞入 Header
        const token = localStorage.getItem('auth_token');
        if (token) {
            config.headers['Authorization'] = `Bearer ${token}`;
        }
        return config;
    },
    (error) => {
        return Promise.reject(error);
    }
);
// 3. 响应拦截器：在收到响应后做的事情
service.interceptors.response.use(
    (response) => {
        const res = response.data;

        // 如果业务状态码不是 0 (假设你的 C++ 后端 0 是成功)，则判定为错误
        if (res.code !== 0) {
            // 在这里可以统一弹窗提示错误，比如使用你项目里的 Toast
            console.error('业务错误:', res.msg || '未知错误');
            return Promise.reject(new Error(res.msg || 'Error'));
        }
        return res; // 直接返回业务数据，组件里不用再写 .data
    },
    (error) => {
        // 处理 HTTP 状态码错误
        let message = '网络请求出错';
        if (error.response) {
            switch (error.response.status) {
                case 401: message = '登录已过期，请重新登录'; break;
                case 403: message = '拒绝访问'; break;
                case 404: message = '请求地址不存在'; break;
                case 500: message = '服务器内部错误'; break;
            }
        }
        console.error(message);
        return Promise.reject(error);
    }
);
export default service;