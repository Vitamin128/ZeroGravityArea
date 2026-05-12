import { createRouter, createWebHistory } from 'vue-router'
import MainSearch from '../views/MainSearch.vue'

const routes = [
  {
    path: '/',
    redirect: '/search'
  },
  {
    path: '/search',
    name: 'Search',
    component: MainSearch
  },
  {
    path: '/history',
    name: 'History',
    // 暂时重定向回搜索，直到你创建了 HistoryView
    component: MainSearch 
  },
  {
    path: '/about',
    name: 'About',
    // 暂时重定向回搜索，直到你创建了 AboutView
    component: MainSearch
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

export default router
