final class ComplexModificationsFileImport: ObservableObject {
  static let shared = ComplexModificationsFileImport()

  var task: URLSessionTask?

  @Published var fetching: Bool = false
  @Published var url: URL?
  @Published var error: String?
  @Published var jsonData: Data?
  @Published var title: String = ""
  @Published var descriptions: [String] = []

  public func fetchJson(_ url: URL) {
    task?.cancel()

    self.url = url
    error = nil
    jsonData = nil
    title = ""
    descriptions = []

    task = URLSession.shared.dataTask(with: url) { [weak self] (data, response, error) in
      DispatchQueue.main.async {
        guard let self = self else { return }
        guard let data = data else { return }

        self.fetching = false

        if let error = error {
          self.error = error.localizedDescription
        } else {
          do {
            self.jsonData = data
            let json = try JSONSerialization.jsonObject(with: data) as? [String: Any]

            self.title = json?["title"] as! String
            for rule in (json?["rules"] as! [[String: Any]]) {
              self.descriptions.append(rule["description"] as! String)
            }
          } catch {
            self.jsonData = nil
            self.error = error.localizedDescription
          }
        }
      }
    }

    fetching = true
    task?.resume()
  }

  public func save() {
    if let data = self.jsonData {
      let directory = String(cString: libkrbn_get_user_complex_modifications_assets_directory())
      let time = Int(NSDate().timeIntervalSince1970)
      let path = URL(fileURLWithPath: "\(directory)/\(time).json")

      do {
        try data.write(to: path)
      } catch {
        self.error = error.localizedDescription
      }
    }
  }
}
