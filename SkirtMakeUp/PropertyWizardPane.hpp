/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROPERTYWIZARDPANE_HPP
#define PROPERTYWIZARDPANE_HPP

#include "PropertyHandler.hpp"
#include <QWidget>

////////////////////////////////////////////////////////////////////

/** PropertyWizardPane is the base class for all wizard panes handling simulation item properties.
    It offers common functionality, such as connecting the propertyValidChanged() signal to the
    target object, and retaining a reference to the relevant property handler so that it does not
    get deleted until the wizard pane is destroyed. */
class PropertyWizardPane : public QWidget
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor retains a reference to the specified property handler so
        that it does not get deleted until the wizard pane is destroyed, and connects the
        propertyValidChanged() and propertyValueChanged() signals to the specified target object. */
    explicit PropertyWizardPane(PropertyHandlerPtr handler, QObject* target);

    // ==================== Event Handling ====================

    /** This function ensures that the first focus-enabled widget in the pane receives the focus
        when the pane is shown. */
protected:
    void showEvent(QShowEvent* event);

signals:
    /** This signal is emitted when the valid state of the property wizard pane (indicating whether
        the wizard is allowed to advance) may have changed. */
    void propertyValidChanged(bool valid);

    /** This signal is emitted when the value of the property being handled by this property wizard
        pane has changed. The signal should only be emitted when the value actually did change, not
        just when it might have changed. Thus the caller should check the previous property value. */
    void propertyValueChanged();

    // ============== Access to data from subclasses ==========

protected:
    /** This template function dynamically casts the handler retained by this wizard pane to the
        specified template type, and returns the result. If the handler is not of the specified
        type, the function returns null. */
    template <class T> T* handlerCast() { return _handler.dynamicCast<T>().data(); }

    /** This function stores a flag in the target simulation item to indicate whether the user has
        configured the property being handled during this session, depending on the value of the
        specified argument. If the argument is omitted, the default value of true is used. */
    void setPropertyConfigured(bool configured=true);

    /** This function returns true if the setPropertyConfigured() was called during this session
        for this combination of target simulation item and property, and false otherwise. In other
        words, it returns true if the property being handled has been configured by the user for
        the target simulation item. */
    bool isPropertyConfigured();

    // ================== Data Members ====================

private:
    PropertyHandlerPtr _handler;
};

////////////////////////////////////////////////////////////////////

#endif // PROPERTYWIZARDPANE_HPP
