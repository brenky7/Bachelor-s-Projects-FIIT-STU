
-- ---------------------------------------------------------------------------------- --
-- ---------------- Trigger for Proces 1: Settting 'exhibited' state ---------------- --
-- ---------------------------------------------------------------------------------- --


CREATE OR REPLACE FUNCTION update_exemplar_state()
RETURNS TRIGGER AS $$
BEGIN
    -- Check if the real_start_date is in the past
    IF NEW.real_start_date < CURRENT_TIMESTAMP THEN
        -- Update the state of exemplars associated with the current exposition
        UPDATE exemplars
        SET state = 'exhibited',
            storage_location = NULL
        WHERE id IN (
            SELECT exemplar_id
            FROM exposition_exemplar_zone
            WHERE exposition_id = NEW.id
        );
    END IF;
    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER exposition_real_start_trigger
AFTER UPDATE OF real_start_date ON expositions
FOR EACH ROW
WHEN (OLD.real_start_date IS NULL AND NEW.real_start_date < CURRENT_TIMESTAMP)
EXECUTE FUNCTION update_exemplar_state();


