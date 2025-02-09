import { Card } from './Card'

interface SkeletonBaseProps {
  className?: string
  width?: string | number
  height?: string | number
}

export function Skeleton({ className = '', width = '100%', height = '1rem' }: SkeletonBaseProps) {
  const style = {
    width: typeof width === 'number' ? `${width}px` : width,
    height: typeof height === 'number' ? `${height}px` : height,
  }
  
  return (
    <div 
      className={`bg-bg-accent rounded animate-pulse ${className}`}
      style={style}
    />
  )
}

interface SkeletonRowProps extends SkeletonBaseProps {
  layout?: 'list' | 'table' | 'settings'
  columns?: number
  labelWidth?: string | number
  valueWidth?: string | number
}

export function SkeletonRow({ 
  className = '', 
  layout = 'list',
  columns = 1,
  labelWidth = 112,
  valueWidth = 96,
  height = 20 
}: SkeletonRowProps) {
  if (layout === 'settings') {
    return (
      <div className={`flex justify-between items-center ${className}`}>
        <Skeleton width={labelWidth} height={height} />
        <Skeleton width={valueWidth} height={height} />
      </div>
    )
  }

  if (layout === 'table') {
    return (
      <div className={`grid grid-cols-12 gap-4 ${className}`}>
        {Array.from({ length: columns }).map((_, j) => (
          <Skeleton 
            key={j}
            height={height}
            className={`col-span-${Math.floor(12 / columns)}`}
          />
        ))}
      </div>
    )
  }

  // Default list layout
  return (
    <div className={`flex justify-between items-center ${className}`}>
      <Skeleton width={labelWidth} height={height} />
      <Skeleton width={valueWidth} height={height} />
    </div>
  )
}

interface SkeletonCardProps {
  title?: string
  rows?: number
  className?: string
  layout?: 'list' | 'table' | 'settings'
  columns?: number
  labelWidth?: string | number
  valueWidth?: string | number
  rowHeight?: number
}

export function SkeletonCard({ 
  title, 
  rows = 3, 
  className = '',
  layout = 'list',
  columns = 1,
  labelWidth = layout === 'settings' ? 128 : 112,
  valueWidth = layout === 'settings' ? 96 : 64,
  rowHeight = 20
}: SkeletonCardProps) {
  return (
    <Card title={title} className={className}>
      <div className="space-y-4">
        {Array.from({ length: rows }).map((_, i) => (
          <SkeletonRow 
            key={i}
            layout={layout}
            columns={columns}
            labelWidth={labelWidth}
            valueWidth={valueWidth}
            height={rowHeight}
          />
        ))}
      </div>
    </Card>
  )
}
