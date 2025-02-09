interface StatRowProps {
  label: string
  value: string | number
  type?: 'status' | 'info'
  className?: string
}

export function StatRow({ label, value, type = 'info', className = '' }: StatRowProps) {
  return (
    <div className={`flex justify-between items-center ${className}`}>
      <span className="text-text-secondary">{label}</span>
      {type === 'status' ? (
        <div className="flex items-center">
          <div className={`h-2.5 w-2.5 rounded-full ${
            String(value).toLowerCase() === 'online' || String(value).toLowerCase() === 'active' 
              ? 'bg-success' 
              : 'bg-warning'
          } animate-pulse mr-2`}></div>
          <span className="text-text-primary capitalize">{value}</span>
        </div>
      ) : (
        <span className="text-text-primary">{value}</span>
      )}
    </div>
  )
}
