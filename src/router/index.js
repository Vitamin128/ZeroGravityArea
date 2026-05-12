import { createRouter, createWebHistory } from 'vue-router'

// 导入所有视图组件
import MainSearch from '../views/MainSearch.vue'
import Gaming from '../views/Gaming.vue'
import AIFriend from '../views/AIFriend.vue'
import IMessage from '../views/IMessage.vue'
import Video from '../views/Video.vue'
import MainPage from '../views/MainPage.vue'

const routes = [
  {
    path: '/',
    redirect: '/search'
  },
  {
    path: '/gameing',
    name: 'Game',
    component: Gaming
  },
  {
    path: '/search',
    name: 'Search',
    component: MainSearch
  },
  {
    path: '/aifriend',
    name: 'AIFriend',
    component: AIFriend
  },
  {
    path: '/message',
    name: 'Message',
    component: IMessage
  },
  {
    path: '/video',
    name: 'Video',
    component: Video
  },
  {
    path: '/mainpage',
    name: 'MainPage',
    component: MainPage
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

export default router
