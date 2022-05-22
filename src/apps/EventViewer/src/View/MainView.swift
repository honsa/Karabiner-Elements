import SwiftUI

struct MainView: View {
  @ObservedObject var eventHistory = EventHistory.shared
  @State private var textInput: String = "Press the key you want to investigate."

  var body: some View {
    VStack(alignment: .leading, spacing: 12.0) {
      GroupBox(label: Text("Text input test field")) {
        VStack {
          TextEditor(
            text: $textInput
          )
          .frame(height: 60)
          .disableAutocorrection(true)
        }
        .padding(6.0)
      }

      GroupBox(label: Text("Keyboard & pointing events")) {
        VStack(alignment: .leading, spacing: 6.0) {
          if eventHistory.entries.count == 0 {
            Divider()
            Spacer()
          } else {
            HStack(alignment: .center, spacing: 12.0) {
              Button(action: {
                eventHistory.copyToPasteboard()
              }) {
                Label("Copy to pasteboard", systemImage: "arrow.right.doc.on.clipboard")
              }

              Button(action: {
                eventHistory.clear()
              }) {
                Label("Clear", systemImage: "clear")
              }

              Spacer()

              if eventHistory.simpleModificationJsonString.count > 0 {
                Button(action: {
                  eventHistory.addSimpleModification()
                }) {
                  Label(eventHistory.addSimpleModificationButtonText, systemImage: "plus.circle")
                }
              }
            }

            ScrollViewReader { proxy in
              ScrollView {
                VStack(alignment: .leading, spacing: 0.0) {
                  ForEach($eventHistory.entries) { $entry in
                    HStack(alignment: .center, spacing: 0) {
                      Text(entry.eventType)
                        .font(.title)
                        .frame(width: 70, alignment: .leading)

                      VStack(alignment: .leading, spacing: 0) {
                        Text(entry.name)
                        Text(entry.misc)
                          .font(.caption)
                      }

                      Spacer()

                      VStack(alignment: .trailing, spacing: 0) {
                        if entry.usagePage.count > 0 {
                          HStack(alignment: .bottom, spacing: 0) {
                            Text("usage page: ")
                              .font(.caption)
                            Text(entry.usagePage)
                          }
                        }
                        if entry.usage.count > 0 {
                          HStack(alignment: .bottom, spacing: 0) {
                            Text("usage: ")
                              .font(.caption)
                            Text(entry.usage)
                          }
                        }
                      }
                      .frame(alignment: .leading)
                    }
                    .padding(.horizontal, 12.0)

                    Divider().id("divider \(entry.id)")
                  }

                  Spacer()
                }
              }
              .background(Color(NSColor.textBackgroundColor))
              .onAppear {
                if let last = eventHistory.entries.last {
                  proxy.scrollTo("divider \(last.id)", anchor: .bottom)
                }
              }
              .onChange(of: eventHistory.entries) { newEntries in
                if let last = newEntries.last {
                  proxy.scrollTo("divider \(last.id)", anchor: .bottom)
                }
              }
            }
          }
        }
        .padding(6.0)
      }
    }
    .padding()
  }
}

struct MainView_Previews: PreviewProvider {
  static var previews: some View {
    MainView()
  }
}
