import SwiftUI

enum LogLevel {
  case info
  case warn
  case error
}

private func callback(
  _ logLines: UnsafeMutableRawPointer?,
  _ context: UnsafeMutableRawPointer?
) {
  if logLines == nil { return }
  if context == nil { return }

  let obj: LogMessages! = unsafeBitCast(context, to: LogMessages.self)

  var logMessageEntries: [LogMessageEntry] = []
  let size = libkrbn_log_lines_get_size(logLines)
  for i in 0..<size {
    let line = libkrbn_log_lines_get_line(logLines, i)
    if line != nil {
      var logLevel = LogLevel.info
      if libkrbn_log_lines_is_warn_line(line) {
        logLevel = LogLevel.warn
      }
      if libkrbn_log_lines_is_error_line(line) {
        logLevel = LogLevel.error
      }

      logMessageEntries.append(
        LogMessageEntry(
          text: String(cString: line!),
          logLevel: logLevel,
          dateNumber: libkrbn_log_lines_get_date_number(line)))
    }
  }

  DispatchQueue.main.async { [weak obj] in
    guard let obj = obj else { return }

    obj.setEntries(logMessageEntries)
  }
}

public class LogMessageEntry: Identifiable, Equatable {
  public var id = UUID()
  public var text = ""
  public var dateNumber: UInt64
  public var foregroundColor = Color.primary
  public var backgroundColor = Color.clear

  init(text: String, logLevel: LogLevel, dateNumber: UInt64) {
    self.text = text
    self.dateNumber = dateNumber

    switch logLevel {
    case LogLevel.info:
      break
    case LogLevel.warn:
      foregroundColor = Color.warningForeground
      backgroundColor = Color.warningBackground
    case LogLevel.error:
      foregroundColor = Color.errorForeground
      backgroundColor = Color.errorBackground
    }
  }

  public static func == (lhs: LogMessageEntry, rhs: LogMessageEntry) -> Bool {
    lhs.id == rhs.id
  }
}

public class LogMessages: ObservableObject {
  public static let shared = LogMessages()

  @Published var entries: [LogMessageEntry] = []
  @Published var currentTimeString = ""

  private var dividers: [LogMessageEntry] = []
  private var timer: Timer?

  init() {}

  deinit {
    unwatch()
  }

  public func watch() {
    entries = []

    let obj = unsafeBitCast(self, to: UnsafeMutableRawPointer.self)
    libkrbn_enable_log_monitor(callback, obj)

    //
    // Create timer
    //

    timer = Timer.scheduledTimer(
      withTimeInterval: 1.0,
      repeats: true
    ) { [weak self] (_: Timer) in
      guard let self = self else { return }

      let formatter = DateFormatter()
      formatter.locale = Locale(identifier: "en_US_POSIX")
      formatter.dateFormat = "[yyyy-MM-dd HH:mm:ss]"
      self.currentTimeString = formatter.string(from: Date())
    }
  }

  public func unwatch() {
    libkrbn_disable_log_monitor()
  }

  public func setEntries(_ entries: [LogMessageEntry]) {
    var newEntries: [LogMessageEntry] = []

    //
    // Remove old dividers
    //

    while dividers.count > 0,
      entries.count > 0,
      dividers[0].dateNumber < entries[0].dateNumber
    {
      dividers.removeFirst()
    }

    //
    // Merge entries and dividers
    //

    var dividerIndex = 0
    entries.forEach { e in
      while dividerIndex < dividers.count,
        dividers[dividerIndex].dateNumber < e.dateNumber
      {
        newEntries.append(dividers[dividerIndex])
        dividerIndex += 1
      }
      newEntries.append(e)
    }

    while dividerIndex < dividers.count {
      newEntries.append(dividers[dividerIndex])
      dividerIndex += 1
    }

    self.entries = newEntries
  }

  public func addDivider() {
    let formatter = DateFormatter()
    formatter.locale = Locale(identifier: "en_US_POSIX")
    formatter.dateFormat = "yyyyMMddHHmmssSSS"
    let dateNumberString = formatter.string(from: Date())

    if let dateNumber = UInt64(dateNumberString) {
      let entry = LogMessageEntry(
        text: String(repeating: "-", count: 80),
        logLevel: LogLevel.info,
        dateNumber: dateNumber)
      entry.foregroundColor = Color(NSColor.textBackgroundColor)
      entry.backgroundColor = Color(NSColor.textColor)

      dividers.append(entry)
      entries.append(entry)
    }
  }
}
