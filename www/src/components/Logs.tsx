import { useState } from 'preact/hooks'
import { Card } from './shared/Card'
import { LoadingButton } from './shared/LoadingButton'
import { PageHeader } from './shared/PageHeader'
import { Select } from './shared/Select'
import { ErrorAlert } from './shared/ErrorAlert'
import { useAsyncData } from '../hooks/useAsyncData'
import { SkeletonCard } from './shared/Skeleton'
import { fetchLogs } from '../api/logs'
import { LogEntry } from '../types/logs'

interface Props {
  path?: string
  MenuButton: () => JSX.Element
}

const LOG_FILTER_OPTIONS = [
  { value: 'all', label: 'All Logs' },
  { value: 'info', label: 'Info' },
  { value: 'warning', label: 'Warnings' },
  { value: 'error', label: 'Errors' }
]

const LEVEL_COLORS = {
  info: 'text-primary',
  warning: 'text-warning',
  error: 'text-danger'
} as const

function LogsSkeleton() {
  return (
    <SkeletonCard 
      title="System Logs" 
      rows={6}
      layout="table"
      columns={3}
      className="space-y-4"
    />
  )
}

export function Logs({ MenuButton }: Props) {
  const { 
    data: logs, 
    error, 
    loading,
    refreshing,
    refresh: loadLogs
  } = useAsyncData(fetchLogs)

  const headerActions = (
    <LoadingButton
      onClick={loadLogs}
      loading={refreshing}
      loadingText="Refreshing..."
      text="Refresh"
    />
  )

  return (
    <div>
      <PageHeader 
        title="System Logs"
        MenuButton={MenuButton} 
        actions={headerActions} 
      />

      <ErrorAlert error={error} className="mb-4" />

      {loading ? (
        <LogsSkeleton />
      ) : logs && Array.isArray(logs) ? (
        <Card>
          <div className="overflow-x-auto scrollbar scrollbar-thumb-bg-accent scrollbar-track-bg-secondary">
            <div className="p-4 border-b border-bg-accent grid grid-cols-12 text-text-secondary text-sm font-medium min-w-[640px]">
              <div className="col-span-3 sm:col-span-2">Timestamp</div>
              <div className="col-span-2 sm:col-span-1">Level</div>
              <div className="col-span-7 sm:col-span-9">Message</div>
            </div>
            <div className="divide-y divide-bg-accent">
              {logs.map(log => (
                <div key={log.id} className="p-4 grid grid-cols-12 items-center hover:bg-bg-accent transition-colors min-w-[640px]">
                  <div className="col-span-3 sm:col-span-2 text-text-secondary text-sm">{log.timestamp}</div>
                  <div className={`col-span-2 sm:col-span-1 ${LEVEL_COLORS[log.level]} font-medium`}>
                    {log.level.charAt(0).toUpperCase() + log.level.slice(1)}
                  </div>
                  <div className="col-span-7 sm:col-span-9 text-text-primary font-mono text-sm whitespace-pre-wrap">{log.message}</div>
                </div>
              ))}
            </div>
          </div>
        </Card>
      ) : null}
    </div>
  )
}
