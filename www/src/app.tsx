import { useEffect, useState } from 'preact/hooks'
import { Router, Link, Route } from 'preact-router'
import { Dashboard } from './components/Dashboard'
import { Settings } from './components/Settings'
import { Logs } from './components/Logs'

interface Props {
  path?: string
}

export function App() {
  const [isMobileMenuOpen, setIsMobileMenuOpen] = useState(false)

  useEffect(() => {
    // Enable dark mode by default
    document.documentElement.classList.add('dark')
  }, [])

  const MenuButton = () => (
    <button
      onClick={() => setIsMobileMenuOpen(!isMobileMenuOpen)}
      className="p-2 rounded-md bg-bg-secondary border border-bg-accent sm:hidden hover:bg-bg-accent"
    >
      <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        {isMobileMenuOpen ? (
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
        ) : (
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h16" />
        )}
      </svg>
    </button>
  )

  return (
    <div className="min-h-screen bg-bg-primary text-text-primary scrollbar scrollbar-thumb-bg-accent scrollbar-track-bg-secondary">
      {/* Overlay for mobile menu */}
      {isMobileMenuOpen && (
        <div
          className="fixed inset-0 bg-black bg-opacity-50 z-30 sm:hidden"
          onClick={() => setIsMobileMenuOpen(false)}
        />
      )}

      {/* Sidebar */}
      <aside className={`fixed inset-y-0 left-0 w-64 bg-bg-secondary border-r border-bg-accent transform transition-transform duration-300 ease-in-out z-40 ${
        isMobileMenuOpen ? 'translate-x-0' : '-translate-x-full sm:translate-x-0'
      }`}>
        <div className="p-6">
          <h2 className="text-2xl font-bold text-text-primary">
            Gateway
            <span className="text-primary ml-2">Control</span>
          </h2>
          <nav className="mt-8 space-y-2">
            <Link 
              href="/" 
              className="flex items-center px-4 py-3 text-text-secondary hover:bg-bg-accent hover:text-text-primary rounded-md transition-colors"
              onClick={() => setIsMobileMenuOpen(false)}
            >
              <span className="text-primary mr-3">◆</span>
              Dashboard
            </Link>
            <Link 
              href="/settings" 
              className="flex items-center px-4 py-3 text-text-secondary hover:bg-bg-accent hover:text-text-primary rounded-md transition-colors"
              onClick={() => setIsMobileMenuOpen(false)}
            >
              <span className="text-secondary mr-3">◆</span>
              Settings
            </Link>
            <Link 
              href="/logs" 
              className="flex items-center px-4 py-3 text-text-secondary hover:bg-bg-accent hover:text-text-primary rounded-md transition-colors"
              onClick={() => setIsMobileMenuOpen(false)}
            >
              <span className="text-text-muted mr-3">◆</span>
              Logs
            </Link>
          </nav>
        </div>
      </aside>

      {/* Main content */}
      <main className="sm:ml-64 p-4 sm:p-8 min-h-screen sm:h-screen overflow-y-auto scrollbar scrollbar-thumb-bg-accent scrollbar-track-bg-secondary">
        <Router>
          <Route path="/" component={() => <Dashboard MenuButton={MenuButton} />} />
          <Route path="/settings" component={() => <Settings MenuButton={MenuButton} />} />
          <Route path="/logs" component={() => <Logs MenuButton={MenuButton} />} />
        </Router>
      </main>
    </div>
  )
}
