project(org_mitk_gui_qt_igtnavigation)

MACRO_CREATE_MITK_CTK_PLUGIN(
  EXPORT_DIRECTIVE IGTNAVIGATION_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDENCIES QmitkExt MitkIGT MitkIGTUI
)