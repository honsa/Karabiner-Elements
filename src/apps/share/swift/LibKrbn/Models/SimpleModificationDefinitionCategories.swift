import Foundation

extension LibKrbn {
  struct SimpleModificationDefinitionCategories: Identifiable {
    var id = UUID()

    var categories: [SimpleModificationDefinitionCategory] = []

    func findLabel(jsonString: String) -> String {
      if let compactJsonString = SimpleModification.formatCompactJsonString(string: jsonString) {
        for category in categories {
          for entry in category.entries {
            if entry.json == compactJsonString {
              return entry.label
            }
          }
        }

        if compactJsonString == "{}" || compactJsonString == "[]" {
          return "---"
        }

        return compactJsonString
      }

      return jsonString
    }
  }
}
